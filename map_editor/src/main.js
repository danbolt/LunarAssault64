
const MAP_WIDTH  = 128
const MAP_HEIGHT  = 128

const GROUND_MAX_HEIGHT = 50;

const makeMappingString = (x, y) => { return (x + '-' + y) };

const EditScreen = function () {
    this.tiles = [];
    this.heights = []

    this.mapping = {};

    this.brushSize = 1;
    this.brushIndex = 0;

    this.isDraggingTopography = false;
    this.topographyDragSpot = { x: 0, y: 0};
    this.topographyReferenceHeight = 0;
};
EditScreen.prototype.init = function() {
  this.tiles = new Array(MAP_WIDTH);
  this.heights = new Array(MAP_WIDTH);
  for (let i = 0; i < MAP_WIDTH; i++) {
    this.tiles[i] = new Array(MAP_HEIGHT);
    this.heights[i] = new Array(MAP_HEIGHT);


    for (let j = 0; j < MAP_HEIGHT; j++) {
        this.tiles[i][j] = 0;
        this.heights[i][j] = 0;
    }
  }
};
EditScreen.prototype.preload = function() {
    this.load.bitmapFont('century', 'century_0.png', 'century.fnt');

    this.load.spritesheet('test_tiles', '../doc/gimp/moon_geo.png', { frameWidth: 8, frameHeight: 8 });
};
EditScreen.prototype.clearTiles = function() {
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            this.tiles[i][j] = 0;
            this.heights[i][j] = 0x00;
        }
    }
};
EditScreen.prototype.refreshTile = function(x, y, value) {
    const mapstring = makeMappingString(x, y);
    this.tiles[x][y] = value & 0xFF;
};

EditScreen.prototype.saveMapToBinary = function() {
    const output = new ArrayBuffer(MAP_WIDTH * MAP_HEIGHT);
    const view = new Uint8ClampedArray(output);

    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            const index = i + (j * MAP_WIDTH);

            view.set([this.tiles[i][j] & 0xFF], index);
        }
    }

    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";

    const name = 'map.bin'
    var blob = new Blob([view], {type: "application/octet-stream"});
    var url = window.URL.createObjectURL(blob);
    a.href = url;
    a.download = name;
    a.click();
    window.URL.revokeObjectURL(url);
};

EditScreen.prototype.saveHeightToBinary = function() {
    const output = new ArrayBuffer(MAP_WIDTH * MAP_HEIGHT);
    const view = new Uint8ClampedArray(output);

    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            const index = i + (j * MAP_WIDTH);

            view.set([this.heights[i][j] & 0xFF], index);
        }
    }

    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";

    const name = 'height.bin'
    var blob = new Blob([view], {type: "application/octet-stream"});
    var url = window.URL.createObjectURL(blob);
    a.href = url;
    a.download = name;
    a.click();
    window.URL.revokeObjectURL(url);
};


EditScreen.prototype.setup3D = function() {
    this.three = {};

    this.three.camera = new THREE.PerspectiveCamera( 60, this.game.canvas.width / this.game.canvas.height,  0.1, 1000 );
    // this.three.camera = new THREE.OrthographicCamera( this.game.canvas.width / - 2, this.game.canvas.width / 2, this.game.canvas.height / 2, this.game.canvas.height / - 2, 1, 1000 );

    this.three.scene = new THREE.Scene();
    this.three.renderer = new THREE.WebGLRenderer( { canvas: this.game.canvas, context: this.game.context, antialias: false } );
    this.three.renderer.autoClear = true;
    this.three.renderer.setClearColor(new THREE.Color(0x330044), 1.0);

    this.three.controls = new THREE.OrbitControls( this.three.camera, this.three.renderer.domElement );
    this.three.controls.enabled = false;

    this.three.raycaster = new THREE.Raycaster(undefined, undefined, 0.1, 200);

    this.three.view = this.add.extern();
    const that = this;
    this.three.view.render = function (prenderer, pcamera, pcalcMatrix) {
        that.three.renderer.state.reset();
        that.three.renderer.render(that.three.scene, that.three.camera);
    }
};
EditScreen.prototype.sampleHeight = function (x, y) {
    x = (~~(x));
    y = (~~(y));
    x = (x + MAP_WIDTH) % MAP_WIDTH;
    y = (y + MAP_HEIGHT) % MAP_HEIGHT;

    return ~~(this.heights[x][y]) & 0xFF;
};
EditScreen.prototype.updateGeoFromHeights = function () {
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            const mid = this.sampleHeight(i, j);
            const n = this.sampleHeight(i, j-1);
            const s = this.sampleHeight(i, j+1);
            const e = this.sampleHeight(i+1, j);
            const w = this.sampleHeight(i-1, j);
            const ne = this.sampleHeight(i+1, j-1);
            const se = this.sampleHeight(i+1, j+1);
            const nw = this.sampleHeight(i-1, j-1);
            const sw = this.sampleHeight(i-1, j+1);
            const heightA = (((nw + n + mid + w) * 0.25));
            const heightB = (((n + ne + e + mid) * 0.25));
            const heightC = (((mid + e + se + s) * 0.25));
            const heightD = (((w + mid + s + sw) * 0.25));

            this.groundGeo.vertices[i + (j * 129)].z = (heightA / 255) * GROUND_MAX_HEIGHT;
            this.groundGeo.vertices[i + (j * 129) + 1].z = (heightB / 255) * GROUND_MAX_HEIGHT;
            this.groundGeo.vertices[i + (j * 129) + 129].z = (heightC / 255) * GROUND_MAX_HEIGHT;
            this.groundGeo.vertices[i + (j * 129) + 129 + 1].z = (heightD / 255) * GROUND_MAX_HEIGHT;
        }
    }
    this.groundGeo.verticesNeedUpdate = true;
};
EditScreen.prototype.updateFaceFromTiles = function () {
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            const tileIndex = this.tiles[i][j];
            const u = ((tileIndex % 4) * 8) / 32.0 ;
            const uPrime = (((tileIndex % 4) * 8) + 8) / 32.0 ;
            const v = 1.0 - (((~~(tileIndex / 4)) * 8) / 32.0) ;
            const vPrime = 1.0 - (((~~(tileIndex / 4)) * 8 + 8) / 32.0) ;

            let facesIndex = ~~((i + (j * MAP_WIDTH)) * 2);
            const face1 = this.groundGeo.faceVertexUvs[0][facesIndex];
            face1[0].set(u, v);
            face1[1].set(u, vPrime);
            face1[2].set(uPrime, v);

            const face2 = this.groundGeo.faceVertexUvs[0][facesIndex + 1];
            face2[0].set(u, vPrime);
            face2[1].set(uPrime, vPrime);
            face2[2].set(uPrime, v);
        }
    }

    this.groundGeo.uvsNeedUpdate = true;
};

EditScreen.prototype.setupScene = function() {
    this.groundMeshes = {};
    var texture = new THREE.TextureLoader().load( '../doc/gimp/moon_geo.png' );
    texture.minFilter = THREE.NearestFilter;
    texture.magFilter = THREE.NearestFilter;

    const groundMat = new THREE.MeshBasicMaterial( { map: texture } );
    const groundGeo = new THREE.PlaneGeometry( MAP_WIDTH, MAP_HEIGHT, MAP_WIDTH, MAP_HEIGHT);
    const groundMesh = new THREE.Mesh( groundGeo, groundMat);
    this.three.scene.add(groundMesh);
    this.groundGeo = groundGeo;
    console.log(groundGeo);

    this.three.camera.position.set(0, -50, 70);
    this.three.camera.lookAt(0, 0, 0);

    this.updateGeoFromHeights();
    this.updateFaceFromTiles();
};

function clamp(num, min, max) {
  return num <= min ? min : num >= max ? max : num;
}

const modes = ['paint', 'smooth', 'pull'];

const threeMouseCoordsVector = new THREE.Vector2(0, 0);
const arrayRaycastResults = [];
EditScreen.prototype.create = function() {

    this.editMode = 'pull';
    this.brushIndex = 0;
    this.brushSize = 1;

    this.isDraggingTopography = false;
    this.topographyDragSpot = { x: 0, y: 0};

    this.mapping = {};
    this.clearTiles();

    this.setup3D();
    this.setupScene();

    const brushTest = this.add.bitmapText(0, 32, 'century', 'brush index: 1', 32);
    brushTest.displayOriginX = 0.5;
    brushTest.displayOriginY = 0.5;

    const modeText = this.add.bitmapText(0, 0, 'century', 'pull mode', 32);
    const tabKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.TAB);
    tabKey.on('up', () => {
        this.editMode = modes[(modes.indexOf(this.editMode) + 1) % modes.length];
        modeText.text = this.editMode + ' mode';

        if (this.editMode !== 'pull') {
            this.isDraggingTopography = false;
        }
    })

    const brushSizeText = this.add.bitmapText(0, 64, 'century', 'brush size: 1', 32);
    const brushSizeKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.BACKTICK);
    brushSizeKey.on('down', () => {
        this.brushSize += 2;
        if (this.brushSize > 20) {
            this.brushSize = 1;
        }

        brushSizeText.text = 'brush size: ' + this.brushSize;
    });

    this.controlsKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.C);

    const saveKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.S);
    saveKey.on('down', () => {
        this.saveMapToBinary();
    });
    const saveKey2 = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.D);
    saveKey2.on('down', () => {
        this.saveHeightToBinary();
    });

    const openKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.O);
    openKey.on('down', () => {
        var input = document.createElement('input');
        input.type = 'file';

        input.onchange = e => { 
            const files = e.target.files;
            if (files.length <= 0) {
                return;
            }

            var reader = new FileReader();
            reader.onload = (data) => {
                const view = new Uint8ClampedArray(data.target.result);

                for (let i = 0; i < view.length; i++) {
                    const byteValue = view[i];
                    const x = i % MAP_WIDTH;
                    const y = ~~(i / MAP_WIDTH);

                    this.refreshTile(x, y, byteValue);
                }

                this.updateFaceFromTiles();

            };
            reader.readAsArrayBuffer(files[0])

            document.body.removeChild(input);
        }

        document.body.appendChild(input);  
        input.click();
    });

    const openKey2 = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.P);
    openKey2.on('down', () => {
        var input = document.createElement('input');
        input.type = 'file';

        input.onchange = e => { 
            const files = e.target.files;
            if (files.length <= 0) {
                return;
            }

            var reader = new FileReader();
            reader.onload = (data) => {
                const view = new Uint8ClampedArray(data.target.result);

                for (let i = 0; i < view.length; i++) {
                    const byteValue = view[i];
                    const x = i % MAP_WIDTH;
                    const y = ~~(i / MAP_WIDTH);

                    this.heights[x][y] = byteValue;
                }

                this.updateGeoFromHeights();

            };
            reader.readAsArrayBuffer(files[0])

            document.body.removeChild(input);
        }

        document.body.appendChild(input);  
        input.click();
    });

    const zeroKey = 48;
    const shiftKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SHIFT);
    for (let i = 0; i < 8; i++) {
        const key = this.input.keyboard.addKey(zeroKey + i + 1);
        const b = i;
        key.on('down', () => {
            let ind = b;
            if (shiftKey.isDown) {
                ind += 8;
            }
            this.brushIndex = ind & 0xFF;
            brushTest.text = 'brush index: ' + (ind + 1);
        })
    }
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
        }
    }

    this.input.on('pointermove', (pointer) => {
        // Painting on 2D map
        if (pointer.isDown && (this.editMode == 'paint')) {
            arrayRaycastResults.length = 0;
            const mouseX = pointer.x / this.game.canvas.width;
            const mouseY = 1.0 - (pointer.y / this.game.canvas.height);
            threeMouseCoordsVector.x = (mouseX * 2.0) - 1.0;
            threeMouseCoordsVector.y = (mouseY * 2.0) - 1.0;
            this.three.raycaster.setFromCamera(threeMouseCoordsVector, this.three.camera);
            this.three.raycaster.intersectObjects(this.three.scene.children, false, arrayRaycastResults);
            if ((arrayRaycastResults.length > 0)) {
                const intersectPoint = arrayRaycastResults[0].point;

                const spotX = ~~((intersectPoint.x + 64));
                const spotY = ~~(Math.abs(intersectPoint.y - 64));

                for (let i = 0; i < this.brushSize; i++) {
                    for (let j = 0; j < this.brushSize; j++) {
                        const targetX = spotX + i - ~~(this.brushSize / 2);
                        const targetY = spotY + j - ~~(this.brushSize / 2);
                        if ((targetX < 0) || (targetX >= MAP_WIDTH)) {
                            continue;
                        }
                        if ((targetY < 0) || (targetY >= MAP_HEIGHT)) {
                            continue;
                        }

                        this.tiles[targetX][targetY] = this.brushIndex;
                    }
                }

                this.updateFaceFromTiles();
            }
        }

        if (pointer.isDown && (this.editMode == 'smooth')) {
            arrayRaycastResults.length = 0;
            const mouseX = pointer.x / this.game.canvas.width;
            const mouseY = 1.0 - (pointer.y / this.game.canvas.height);
            threeMouseCoordsVector.x = (mouseX * 2.0) - 1.0;
            threeMouseCoordsVector.y = (mouseY * 2.0) - 1.0;
            this.three.raycaster.setFromCamera(threeMouseCoordsVector, this.three.camera);
            this.three.raycaster.intersectObjects(this.three.scene.children, false, arrayRaycastResults);
            if ((arrayRaycastResults.length > 0)) {
                const intersectPoint = arrayRaycastResults[0].point;

                const spotX = ~~((intersectPoint.x + 64));
                const spotY = ~~(Math.abs(intersectPoint.y - 64));

                let sum = 0;
                for (let i = 0; i < this.brushSize; i++) {
                    for (let j = 0; j < this.brushSize; j++) {
                        const targetX = spotX + i - ~~(this.brushSize / 2);
                        const targetY = spotY + j - ~~(this.brushSize / 2);
                        if ((targetX < 0) || (targetX >= MAP_WIDTH)) {
                            continue;
                        }
                        if ((targetY < 0) || (targetY >= MAP_HEIGHT)) {
                            continue;
                        }

                        sum += this.heights[targetX][targetY];
                    }
                }
                const mean = sum / (this.brushSize * this.brushSize);
                for (let i = 0; i < this.brushSize; i++) {
                    for (let j = 0; j < this.brushSize; j++) {
                        const targetX = spotX + i - ~~(this.brushSize / 2);
                        const targetY = spotY + j - ~~(this.brushSize / 2);
                        if ((targetX < 0) || (targetX >= MAP_WIDTH)) {
                            continue;
                        }
                        if ((targetY < 0) || (targetY >= MAP_HEIGHT)) {
                            continue;
                        }

                        this.heights[targetX][targetY] = (this.heights[targetX][targetY] + mean) / 2;
                    }
                }

                this.updateGeoFromHeights();
            }
        }

        if (this.isDraggingTopography) {
            const verticalDelta = (pointer.downY - pointer.y);

            for (let i = 0; i < this.brushSize; i++) {
                for (let j = 0; j < this.brushSize; j++) {
                    const targetX = this.topographyDragSpot.x + i - ~~(this.brushSize / 2);
                    const targetY = this.topographyDragSpot.y + j - ~~(this.brushSize / 2);
                    if ((targetX < 0) || (targetX >= MAP_WIDTH)) {
                        continue;
                    }
                    if ((targetY < 0) || (targetY >= MAP_HEIGHT)) {
                        continue;
                    }

                    this.heights[targetX][targetY] = ~~(clamp( this.topographyReferenceHeight + verticalDelta, 0, 255)) & 0xff;
                }
            }
            this.updateGeoFromHeights();
        }
    });

    this.input.on('pointerdown', (pointer) => {
        if (this.editMode !== 'pull') {
            this.isDraggingTopography = false;
            return;
        }

        this.isDraggingTopography = false;
        arrayRaycastResults.length = 0;
        const mouseX = pointer.x / this.game.canvas.width;
        const mouseY = 1.0 - (pointer.y / this.game.canvas.height);
        threeMouseCoordsVector.x = (mouseX * 2.0) - 1.0;
        threeMouseCoordsVector.y = (mouseY * 2.0) - 1.0;
        this.three.raycaster.setFromCamera(threeMouseCoordsVector, this.three.camera);
        this.three.raycaster.intersectObjects(this.three.scene.children, false, arrayRaycastResults);
        if ((arrayRaycastResults.length > 0)) {
            // There should be nothing else in the scene
            const intersectPoint = arrayRaycastResults[0].point;
            // console.log(arrayRaycastResults[0]);

            this.topographyDragSpot.x = ~~((intersectPoint.x + 64));
            this.topographyDragSpot.y = ~~(Math.abs(intersectPoint.y - 64));
            this.isDraggingTopography = true;
            this.topographyReferenceHeight = this.heights[this.topographyDragSpot.x][this.topographyDragSpot.y];
        }
    });

    this.input.on('pointerup', (pointer) => {
        this.isDraggingTopography = false;
    });
};
EditScreen.prototype.update = function() {
    this.three.controls.enabled = this.controlsKey.isDown;
};

const main = function() {
    let game = new Phaser.Game({
        width: 640 * 2,
        height: 4 * 128,
        type : Phaser.WEBGL,
        pixelArt: true,
        antialias: false,
        scaleMode: Phaser.Scale.ScaleModes.FIT,
        autoCenter: Phaser.Scale.Center.CENTER_BOTH,
        roundPixels: true,
        input: {
            gamepad: true
        },
        plugins: {
            global: [],
        }
     });

    game.scene.add('EditScreen', EditScreen, true);
};
