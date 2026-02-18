class Tileset {
    constructor(ctxID, tileSize){
        this.tileSize = tileSize;
        this.handle = null;
        this.ctx = null;
        this.tileset = {
            name: 'none',
            img: null,
            width: 0,
            height: 0,
            ready: false
        }
        this.init(ctxID);

        this.hover = {
            x: -1,
            y: -1,
        }

        this.selected = {
            x: -1,
            y: -1,
        }
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
            const pos = this.getGridPos(e);
            this.selected.x = pos.x;
            this.selected.y = pos.y;
        })
    }

    load(imageSrc){
        const newImg = new Image();
        newImg.src = imageSrc;
        newImg.onload = () => {
            this.tileset.img = newImg;
            this.tileset.width = newImg.width;
            this.tileset.height = newImg.height;
            this.tileset.ready = true;
            console.log(`${imageSrc} loaded`);
            this.handle.width = newImg.width;
            this.handle.height = newImg.height;
        }
        this.tileset.name = imageSrc;
    }

    getGridPos(e){
        const x = Math.floor(e.offsetX / this.tileSize) * this.tileSize;
        const y = Math.floor(e.offsetY / this.tileSize) * this.tileSize;
        return { x, y };
    }

    getTilesetImg(){
        return this.tileset.img;
    }

    getSelectedPos(){
        return {
            x: this.selected.x,
            y: this.selected.y
        };
    }

    draw(){
        this.ctx.clearRect(0,0,this.handle.width,this.handle.height);
        if (this.tileset.ready && this.tileset.img) {
            //console.log('d')
            this.ctx.drawImage(
                this.tileset.img,
                0, 0,
                this.tileset.width, this.tileset.height,
                0, 0,
                this.tileset.width, this.tileset.height
            )
        }
        this.drawSelection(this.hover, 'rgb(0,0,0)', 2);
        this.drawSelection(this.selected, 'rgb(0,0,0)', 3);
    }

    drawSelection(rect, color, lineWidth){
        if(rect.x < 0 || rect.y < 0) return;

        this.ctx.strokeStyle = color;
        this.ctx.lineWidth = lineWidth;
        this.ctx.strokeRect(rect.x, rect.y, this.tileSize, this.tileSize);
    }
};