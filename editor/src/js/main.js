const tileset = new Tileset('tileset', TILE_SIZE);
tileset.load('../../assets/img/testTileset2.png')

const map = new Map('map', TILE_SIZE);
map.setSize(20,20);
map.setTileset(tileset);
setTimeout(() => {map.setTilesetImg(tileset.getTilesetImg())},1000);

function mainLoop(){
    tileset.draw()
    map.draw();

    requestAnimationFrame(mainLoop);
}
test()
//mainLoop();


async function test(){
    const mockData = {
        smth2: 'test235626',
        val: '300',
        o: {t: "tt", h: "h1h1"}
    };
    const res = await fetch('http://127.0.0.1:27015/save', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(mockData)
    });
    const text = await res.text()
    console.log(text);
}