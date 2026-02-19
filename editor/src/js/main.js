const tileset = new Tileset('tileset', TILE_SIZE);
tileset.load('assets/img/testTileset2.png')

const map = new Map('map', TILE_SIZE);
map.setSize(20,20);
map.setTileset(tileset);
setTimeout(() => {map.setTilesetImg(tileset.getTilesetImg())},1000);

document.getElementById('bntSave')?.addEventListener('click', e => {
    save();
});

function mainLoop(){
    tileset.draw()
    map.draw();

    requestAnimationFrame(mainLoop);
}
mainLoop();


async function save(){
    const mapInfo = map.exportMap();
    const savefile = {
        map: mapInfo
    };  
    console.log(mapInfo)
    console.log(savefile)
    const res = await fetch('http://127.0.0.1:27015/save', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(savefile)
    });
    const text = await res.text()
    console.log(text);
}