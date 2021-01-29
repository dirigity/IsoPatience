let ready = false;
let h = 17;
let w = 17;
let t = 10000;

importScripts("a.out.js");

onmessage = function (e) {
    if (ready) {
        if (e.data[0] == "NewBoard") {
            Module.__Z8JSconfigiii(h, w, t)//h w t
            let ret = NewBoard()
            postMessage(["NewBoard", ret])
        }
        else if (e.data[0] == "Config") {
            if (e.data[1][0] != -1) h = e.data[1][0];
            if (e.data[1][1] != -1) w = e.data[1][1];
            if (e.data[1][2] != -1) t = e.data[1][2];

            //console.log(h,w,t)

        }

    }

}

function toCell(a) {
    let x = ["#", "+", "x", "~", "%", " ", "?", "G"];
    return x[a];

}

function toDir(a) {
    let x = ["^", "v", ">", "<"];
    return x[a];
}

function NewBoard() { };

Module.onRuntimeInitialized = (_) => {
    NewBoard = () => {
        Module.__Z10JSnewBoardv();
        Module.__Z6JSsyncv();
        let board = [];
        let h = Module.__Z10JSgetBoardv();
        let w = Module.__Z10JSgetBoardv();
        for (let y = 0; y < h; y++) {
            let row = []
            for (let x = 0; x < w; x++) {
                row.push(toCell(Module.__Z10JSgetBoardv()));
            }
            board.push(row)
        }
        let instructions = []
        let l = Module.__Z10JSgetBoardv();
        for (let i = 0; i < l; i++) {
            instructions.push(toDir(Module.__Z10JSgetBoardv()));
        }
        return [board, instructions, Module.__Z10JSgetBoardv(), Module.__Z10JSgetBoardv() == -1];
    }
    ready = true;
    postMessage(["ready"]);
}