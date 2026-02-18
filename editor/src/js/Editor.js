class Editor{
    constructor(ctxTileset, ctxMap){
        const tilesetHandle = this.getHandle(ctxTileset);
        const mapHandle = this.getHandle(ctxMap);

        this.canvas = {
            tileset: {
                handle: tilesetHandle,
                ctx: this.getContext(tilesetHandle)
            },
            map: {
                handle: mapHandle,
                ctx: this.getContext(mapHandle)
            }
        }
        this.tileset = {
            name: 'none',
            img: null,
            width: 0,
            height: 0,
            ready: false
        }
    }

    loadTileset(src){
        const newImg = new Image();
        newImg.src = src;
        newImg.onload = () => {
            this.tileset.img = newImg;
            this.tileset.width = newImg.width;
            this.tileset.height = newImg.height;
            this.tileset.ready = true;
            console.log(`${src} loaded`);
            this.canvas.tileset.handle.width = newImg.width;
            this.canvas.tileset.handle.height = newImg.height;
        }
        this.tileset.name = src;
    }

    getHandle(canvasID){
        const handle = document.getElementById(canvasID);
        if(!handle){
            console.log(`${handle} not found`)
        }
        console.log(`returning ${handle}`)
        return handle;
    }
    getContext(canvasHandle){
        const ctx = canvasHandle.getContext('2d');
        return ctx;
    }

    drawTileset(){
        this.canvas.tileset.ctx.clearRect(0,0,224,224);
        if (this.tileset.ready && this.tileset.img) {
            console.log('d')
            this.canvas.tileset.ctx.drawImage(
                this.tileset.img,
                0, 0,
                this.tileset.width, this.tileset.height,
                0, 0,
                this.tileset.width, this.tileset.height
            )
        }
    }
}