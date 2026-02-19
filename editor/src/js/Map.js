class Map {
    constructor(ctxID, tileSize){
        this.tileSize = tileSize;
        this.handle = null;
        this.ctx = null;

        this.tiles = [];
        this.map = {
            tilesetUsed: null,
            tilesetImgUsed: null,
            widthInTiles: -1,
            heightInTiles: -1
        };

        this.hover = {
            x: -1,
            y: -1,
        }

        this.init(ctxID);
    }

    init(id){
        const tempHandle = document.getElementById(id);
        if(!tempHandle){
            console.log(`couldn't find element of id: ${id}`);
            return;
        }
        this.handle = tempHandle;
        this.ctx = tempHandle.getContext('2d');

        this.handle.addEventListener('mousemove', e => {
            const pos = this.getGridPos(e);
            this.hover.x = pos.x;
            this.hover.y = pos.y;
            //console.log(`x: ${this.hover.x}, y: ${this.hover.y}`)
        });
        this.handle.addEventListener('click', e => {
            if(this.map.tilesetUsed){
                const targetRow = Math.round(this.hover.y / this.tileSize);
                const targetTile = Math.round(this.hover.x / this.tileSize);
                const tilesetPos = this.map.tilesetUsed.getSelectedPos();
                this.tiles[targetRow][targetTile][0] = tilesetPos.x;
                this.tiles[targetRow][targetTile][1] = tilesetPos.y;
            }
        });
    }

    initTiles(widthInTiles, heightInTiles){
        for(let rowNum = 0; rowNum < heightInTiles; rowNum++){
            const newRow = [];
            for(let tileNum = 0; tileNum < widthInTiles; tileNum++){
                newRow.push([-1, -1]);
            }
            this.tiles.push(newRow);
        }
        console.log(this.tiles);
    }

    exportMap(){
        return {
            name: 'map name',
            tiles: this.tiles
        };
    }

    setSize(widthInTiles, heightInTiles){
        if(!this.ctx || !this.handle){
            console.log('Could not set size of the map, ctx or handle is null');
            return;
        }
        this.handle.width = widthInTiles * this.tileSize;
        this.handle.height = heightInTiles * this.tileSize;
        this.map.widthInTiles = widthInTiles;
        this.map.heightInTiles = heightInTiles;
        console.log(`Map size set to ${this.handle.width}x${this.handle.height}`);

        if(!this.tiles.length){
            this.initTiles(widthInTiles, heightInTiles);
        }
    }

    setTilesetImg(img){
        this.map.tilesetImgUsed = img;
    }

    setTileset(tileset){
        this.map.tilesetUsed = tileset;
    }

    getGridPos(e){
        const x = Math.floor(e.offsetX / this.tileSize) * this.tileSize;
        const y = Math.floor(e.offsetY / this.tileSize) * this.tileSize;
        return { x, y };
    }

    draw(){
        if(!this.map.tilesetImgUsed) return;
        this.ctx.clearRect(0,0,this.handle.width,this.handle.height);
        for(let rowNum = 0; rowNum < this.map.heightInTiles; rowNum++){
            for(let tileNum = 0; tileNum < this.map.widthInTiles; tileNum++){
                const targetX = this.tiles[rowNum][tileNum][0];
                const targetY = this.tiles[rowNum][tileNum][1];
                if(targetX < 0 || targetY < 0){
                    continue;
                }
                this.ctx.drawImage(
                    this.map.tilesetImgUsed,
                    targetX, targetY,
                    this.tileSize, this.tileSize,
                    tileNum * this.tileSize, rowNum * this.tileSize,
                    this.tileSize, this.tileSize
                );
            }
        }
        this.drawSelection(this.hover, 'rgb(0,0,0)', 2  );
    }

    drawSelection(rect, color, lineWidth){
        if(rect.x < 0 || rect.y < 0) return;

        this.ctx.strokeStyle = color;
        this.ctx.lineWidth = lineWidth;
        this.ctx.strokeRect(rect.x, rect.y, this.tileSize, this.tileSize);
    }

}