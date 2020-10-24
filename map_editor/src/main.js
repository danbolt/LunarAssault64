
const MAP_WIDTH  = 128
const MAP_HEIGHT  = 128

const makeMappingString = (x, y) => { return (x + '-' + y) };

const EditScreen = function () {
    this.tiles = [];

    this.mapping = {};

    this.brushSize = 1;
    this.brushIndex = 0;
};
EditScreen.prototype.init = function() {
  this.tiles = new Array(MAP_WIDTH);
  for (let i = 0; i < MAP_WIDTH; i++) {
    this.tiles[i] = new Array(MAP_HEIGHT);

    for (let j = 0; j < MAP_HEIGHT; j++) {
        this.tiles[i][j] = 0;
    }
  }
};
EditScreen.prototype.preload = function() {
    this.load.spritesheet('test_tiles', '../doc/gimp/moon_geo.png', { frameWidth: 8, frameHeight: 8 });
};
EditScreen.prototype.clearTiles = function() {
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            this.tiles[i][j] = 0;
        }
    }
};
EditScreen.prototype.refreshTile = function(x, y, value) {
    const mapstring = makeMappingString(x, y);

    this.mapping[mapstring].setFrame(value);
    this.tiles[x][y] = value & 0xFF;
};


EditScreen.prototype.saveToLocalBin = function() {
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
EditScreen.prototype.create = function() {
    const brushTest = this.add.text(128 * 4, 32, 'brush index: 1', { color: 'white', font: 'monospace' });
    brushTest.displayOriginX = 0.5;
    brushTest.displayOriginY = 0.5;


    const saveKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.S);
    saveKey.on('down', () => {
        this.saveToLocalBin();
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
    this.mapping = {};
    this.clearTiles();
    for (let i = 0; i < MAP_WIDTH; i++) {
        for (let j = 0; j < MAP_HEIGHT; j++) {
            const s = this.add.image(i * 4, j * 4, 'test_tiles', 0);
            s.displayOriginX = 0;
            s.displayOriginY = 0;
            s.scaleX = 0.5;
            s.scaleY = 0.5

            this.mapping[makeMappingString(i, j)] = s;
        }
    }

    this.input.on('pointermove', (pointer) => {
        if (pointer.isDown && (pointer.x < (128 * 4))) {
            const scopedX = ~~((pointer.x) / 4.0);
            const scopedY = ~~((pointer.y) / 4.0);

            this.refreshTile(scopedX, scopedY, this.brushIndex);
        }
    });
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
