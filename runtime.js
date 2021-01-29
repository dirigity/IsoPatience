var canvas = document.getElementById("canvas");
var ctx = canvas.getContext("2d");
var keys = [];
var ratony = 0;
var ratonx = 0;
var fps = 60;
var NextRequestEndsAt = -1;
var standarPatience = 10000;
var firstRequestPatience = 10;

var storedLevels = 10;

var LevelBackLog = [];

function rand(params) {
    return Math.random();
}

let playing = false;
let waitingMode = false;
let startOfWaiting = -1;
let currentLevel = [];

function progressBar(p) {
    //console.log(p)
    let centerX = canvas.width / 2;
    let centerY = canvas.height / 2;
    let height_ = 100;
    let width_ = 1000;
    let border = 4;
    ctx.fillStyle = rgb(100, 100, 120);

    ctx.fillRect(centerX - width_ / 2 - border / 2, centerY - height_ / 2 - border / 2, width_ + border, height_ + border);
    ctx.fillStyle = rgb(0, 0, 10);

    ctx.fillRect(centerX - width_ / 2, centerY - height_ / 2, width_ * p, height_);

}

let StartLevel = -1;

let justPoped = true;

let PlayerX = 1;
let PlayerY = 1;
let OrangeFlavour = false;
let tileZ = [];

var movementStart = -1;
var movementLenght = 300;
var movementDirection = "";
var moving = false;

var PlayerXBeguin = -1;
var PlayerYBeguin = -1;
var slide = false;
var movementAditionalZ = 0;
var falling = false;
var acceleration = 0;

function gameTick() {

    canvas.height = canvas.clientHeight;
    canvas.width = canvas.clientWidth;
    ctx.fillStyle = rgb(255, 255, 255);
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    if (!playing && LevelBackLog.length != 0) {
        currentLevel = LevelBackLog.pop();
        PlayerX = 1;
        PlayerY = 1;
        OrangeFlavour = false;
        console.log("poped new level")
        tileZ = [];
        for (let y = 0; y < currentLevel[0].length; y++) {
            let row = [];
            for (let x = 0; x < currentLevel[0][y].length; x++) {
                row.push(0);
            }
            tileZ.push(row);
        }
        console.log(tileZ)

        if (LevelBackLog.length == storedLevels - 1) {
            BoardFinder.postMessage(["Config", [-1, -1, standarPatience]]);
            var d = new Date();
            NextRequestEndsAt = d.getTime() + standarPatience;
            BoardFinder.postMessage(["NewBoard"]);
        }
        playing = true;
        var d = new Date();
        StartLevel = d.getTime();
        waitingMode = false;
    } else if (!playing && !waitingMode) {
        waitingMode = true;
        var d = new Date();

        startOfWaiting = d.getTime();
    }
    if (waitingMode) {
        let totalWatingTime = NextRequestEndsAt - startOfWaiting;
        var d = new Date();

        let timeLeft = NextRequestEndsAt - d.getTime()
        //console.log(startOfWaiting,NextRequestEndsAt,d.getTime())
        progressBar(timeLeft / totalWatingTime);
    }

    if (playing) {
        var d = new Date();

        if (d.getTime() - movementStart > movementLenght) {
            PlayerY = Math.round(PlayerY)
            PlayerX = Math.round(PlayerX)

            if (falling) {
                movementAditionalZ = movementAditionalZ - acceleration;
                acceleration = acceleration + 0.01;
                if (movementAditionalZ < -3) {
                    movementAditionalZ = 0;
                    PlayerX = 1;
                    PlayerY = 1;
                    falling = false;
                }
            } else if (moving) {
                movementAditionalZ = 0;
                slide = false;
                moving = false;
                //console.log(currentLevel[0][PlayerY][PlayerX]);
                if (currentLevel[0][PlayerY][PlayerX] == "%") {
                    OrangeFlavour = true;
                }
                if (currentLevel[0][PlayerY][PlayerX] == "+") {
                    OrangeFlavour = false;
                    slide = true;
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;
                }
                if (currentLevel[0][PlayerY][PlayerX] == " ") {
                    falling = true;
                    if (slide) {
                        acceleration = 0;
                    } else {
                        acceleration = 0.1;
                    }

                }
                if (currentLevel[0][PlayerY][PlayerX] == "G") {
                    playing = false;
                }
                if (currentLevel[0][PlayerY][PlayerX] == "~") {
                    if (currentLevel[0][PlayerY - 1][PlayerX] == "x" || currentLevel[0][PlayerY + 1][PlayerX] == "x" || currentLevel[0][PlayerY][PlayerX - 1] == "x" || currentLevel[0][PlayerY][PlayerX + 1] == "x") {
                        movementStart = d.getTime();
                        PlayerXBeguin = PlayerX;
                        PlayerYBeguin = PlayerY;
                        if (movementDirection == "v") {
                            movementDirection = "^"
                        } else if (movementDirection == "<") {
                            movementDirection = ">"

                        } else if (movementDirection == ">") {
                            movementDirection = "<"

                        } else if (movementDirection == "^") {
                            movementDirection = "v"

                        }
                    }
                }
                if (currentLevel[0][PlayerY][PlayerX] == "x" || (OrangeFlavour && currentLevel[0][PlayerY][PlayerX] == "~")) {
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;
                    if (movementDirection == "v") {
                        movementDirection = "^"
                    } else if (movementDirection == "<") {
                        movementDirection = ">"

                    } else if (movementDirection == ">") {
                        movementDirection = "<"

                    } else if (movementDirection == "^") {
                        movementDirection = "v"

                    }
                }
            } else {
                if (keys[38]) {
                    movementDirection = "^"
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;

                } else if (keys[37]) {
                    movementDirection = "<"
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;

                } else if (keys[39]) {
                    movementDirection = ">"
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;

                } else if (keys[40]) {
                    movementDirection = "v"
                    movementStart = d.getTime();
                    PlayerXBeguin = PlayerX;
                    PlayerYBeguin = PlayerY;
                }
            }



        } else {
            moving = true;
            let movementProgress = (d.getTime() - movementStart) / movementLenght
            movementAditionalZ = (movementProgress - Math.pow(movementProgress, 2)) * (!slide)
            if (movementDirection == "v") {
                PlayerY = PlayerYBeguin + movementProgress
            } else if (movementDirection == "<") {
                PlayerX = PlayerXBeguin - movementProgress

            } else if (movementDirection == ">") {
                PlayerX = PlayerXBeguin + movementProgress

            } else if (movementDirection == "^") {
                PlayerY = PlayerYBeguin - movementProgress

            }
        }

        for (let y = 0; y < tileZ.length; y++) {
            for (let x = 0; x < tileZ[y].length; x++) {
                if (y == PlayerY && x == PlayerX) {
                    var leeway = 0.05;
                    tileZ[y][x] = tileZ[y][x] + (-tileZ[y][x] + leeway) * 0.1

                } else {
                    tileZ[y][x] = tileZ[y][x] * 0.9

                }
            }
        }


        if (!justPoped && keys[83]) {
            playing = false;
            justPoped = true;
        }
        if (!keys[83]) {
            justPoped = false
        }

        let diagonal = Math.sqrt(Math.pow(currentLevel[0].length, 2) + Math.pow(currentLevel[0].length, 2))

        for (let y = 0; y < currentLevel[0].length; y++) {
            for (let x = 0; x < currentLevel[0][y].length; x++) {

                ISOdraw(y, x, tileZ[y][x], currentLevel[0][y][x], diagonal);

                if (Math.ceil(PlayerY) == y && x == Math.ceil(PlayerX)) {
                    playerZ = -0.32 + tileZ[Math.round(PlayerY)][Math.round(PlayerX)] - movementAditionalZ

                    if (!OrangeFlavour) {
                        ISOdraw(PlayerY, PlayerX, playerZ, "Player", diagonal);
                    } else {
                        ISOdraw(PlayerY, PlayerX, playerZ, "PlayerO", diagonal);
                    }
                }
            }
        }


    }



    setTimeout(() => {
        gameTick();
    }, 1000 / fps);
}
function ISOdraw(y, x, z, c, diag) {
    let size = 1.7 * Math.min(canvas.height / diag * 2, canvas.width / diag)

    let XOrigin = canvas.width / 2 + size * diag / 30;
    let YOrigin = 0 //canvas.height/2
    let manualAdj = 0.78;
    let XVector = [size / 4, size / 1.82];
    let YVector = [size / 4, -size / 1.82]

    if (c != " ") {
        var img = document.getElementById(c);
        //console.log(c,img)
        let IsoX = XOrigin + x * XVector[1] + y * YVector[1];
        let IsoY = YOrigin + x * XVector[0] + y * YVector[0] + z * size;

        IsoX *= manualAdj;
        IsoY *= manualAdj;

        ctx.drawImage(img, IsoX, IsoY, size, size);
    }

}

var BoardFinder = new Worker('worker.js');
BoardFinder.onmessage = function (e) {
    var restart = false
    if (e.data[0] == "NewBoard") {
        console.log("new board")

        if (e.data[1][0].length != h || e.data[1][0][0].length != w) {
            console.log("discarding");
            restart = true;
        } else {
            LevelBackLog.push(e.data[1])
            console.log(e.data[1]);
            if (LevelBackLog.length < storedLevels) {
                BoardFinder.postMessage(["Config", [-1, -1, standarPatience]]);
                var d = new Date();
                NextRequestEndsAt = d.getTime() + standarPatience;
                BoardFinder.postMessage(["NewBoard"]);
            }
        }
    }
    if (e.data[0] == "ready" || restart) {
        console.log("worker ready");
        BoardFinder.postMessage(["Config", [-1, -1, firstRequestPatience]])
        var d = new Date();
        NextRequestEndsAt = d.getTime() + firstRequestPatience;
        BoardFinder.postMessage(["NewBoard"])
    }
}


function optionMenu() {
    document.getElementById("options").style.transform = "translateY(80px)";
    document.getElementById("darkOverlay").style.opacity = "0.4"
    document.getElementById("darkOverlay").style.zIndex = "10"

}
function optionMenuClose() {
    document.getElementById("options").style.transform = "translateY(2000px)";
    document.getElementById("darkOverlay").style.opacity = "0"
    setTimeout(() => {
        document.getElementById("darkOverlay").style.zIndex = "-1"
    }, 500);
}

let h = 17;
let w = 17;

function optionMenuApply() {
    w = document.getElementById("widthSelector").value;
    h = document.getElementById("heightSelector").value;
    BoardFinder.postMessage(["Config", [h + 2, w + 2, -1]])
    LevelBackLog = [];
}
function play() {

    canvas.style.height = "100%"
    gameTick();
}


window.onkeyup = function (e) { keys[e.keyCode] = false; }
window.onkeydown = function (e) { keys[e.keyCode] = true; }
function drawXYrect(x, y, grosor, color) {
    //console.log("fufo?")
    if (color)
        ctx.fillStyle = color;
    else
        ctx.fillStyle = "#555555";
    ctx.fillRect(x, y, grosor, grosor);
}
function rgb(r, g, b) {
    var r_ = check(decToHex(r));
    var g_ = check(decToHex(g));
    var b_ = check(decToHex(b));
    return "#" + r_ + g_ + b_;
}

function decToHex(n) {
    if (n < 0) {
        n = 0xFFFFFFFF + n + 1;
    }
    return Math.round(n).toString(16).toUpperCase();
}

function check(n) {
    //console.log(n)
    if (n.length > 2) {
        return "FF";
    } else if (n.length < 2) {
        return "0" + n;

    } else return n
}

canvas.addEventListener('mousemove', function onMouseover(e) {
    ratonx = e.clientX;
    ratony = e.clientY;
});

window.addEventListener('DOMMouseScroll', mouseWheelEvent);
window.addEventListener("wheel", mouseWheelEvent);
function mouseWheelEvent(e) {
    //console.log(e.wheelDelta ? e.wheelDelta : -e.detail);
    var movement = (e.wheelDelta ? e.wheelDelta : -e.detail) / 120;
    if (movement < 0) {
        ScrollUp();
    } else {
        ScrollDown();
    }
}

function ScrollUp() {

}
function ScrollDown() {

}