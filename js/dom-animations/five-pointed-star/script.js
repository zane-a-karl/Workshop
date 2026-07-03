// import {vs, fs} from "./cube.js";
import {vs, fs} from "./star.js";

const BACKGROUND = "#010101";
const FOREGROUND = "#50FF50";

// Can just access window.game or game directly b/c
// ids are auto-exposed by browser but this is safer
const game = document.getElementById("game");
const ctx = game.getContext("2d");
game.width = 800;
game.height = 800;

function clear() {
    ctx.fillStyle = BACKGROUND;
    ctx.fillRect(0, 0, game.width, game.height );
    // ctx.clearRect(0, 0, game.width, game.height);
}

function point({x, y}) {
    const sz = 20;
    ctx.fillStyle = FOREGROUND;
    // render around center not from top left
    // ctx.fillRect(x - sz/2, y - sz/2, sz, sz);
    ctx.beginPath();
    ctx.arc(x, y, sz/2, 0, Math.PI * 2);
    ctx.fill();
}

function line(p1, p2) {
    ctx.lineWidth = 3;
    ctx.strokeStyle = FOREGROUND;
    ctx.beginPath();
    ctx.moveTo(p1.x, p1.y);
    ctx.lineTo(p2.x, p2.y);
    ctx.stroke();
}

// Convert from Cartesean to viewport coords
function screen(p) {
    return {
        // -1..1 => 0..w
        x: game.width * (p.x + 1)/2,
        // -1..1 => 0..2 => 0..1 => 0..h
        y: game.height * (1 - p.y)/2,
    }
}

function project({x, y, z}) {
    return {
        x: x/z,
        y: y/z,
    };
}

function translate_z({x, y, z}, dz) {
    return {x, y, z: z + dz};
}

function translate_y_sinusoidally({x, y, z}, dy, amp) {
    return {x, y: y + amp*Math.sin(dy), z};
}

/**
 * [cos -sin]
 * [sin  cos]
 */
function rotate_xz({x, y, z}, angle) {
    return {
        x: x*Math.cos(angle) - z*Math.sin(angle),
        y,
        z: x*Math.sin(angle) + z*Math.cos(angle),
    };
}

const FPS = 60;
let dz = 2;
let angle = 0;
let osc = 0;
const amp = 0.75;

function frame() {
    const dt = 1/FPS;
    // dz = 100*dt;
    angle += Math.PI *dt;
    osc  = (osc + Math.PI * dt)%(2*Math.PI);
    clear();

    // const pt = { x:  0,    y: 0.35, z: 0.25 };
    // for (const v of vs) {
    //     point(
    //         screen(
    //             project(
    //                 translate_z(
    //                     rotate_xz(v, angle), dz ))));
    // }
    for (const f of fs) {
        for (let i = 0; i < f.length; i++) {
            const p1 = vs[f[i]];
            const p2 = vs[f[(i+1)%f.length]];
            const np1 =
                  screen(
                      project(
                          translate_z(
                              translate_y_sinusoidally(
                                  rotate_xz(p1, angle),
                                  osc, amp),
                              dz)));
            const np2 =
                  screen(
                      project(
                          translate_z(
                              translate_y_sinusoidally(
                                  rotate_xz(p2, angle),
                                  osc, amp),
                              dz)));
            line(np1, np2);
        }
    }
    setTimeout(frame, 1000/FPS);
}

setTimeout(frame, 1000/FPS);
// point(screen({x: -0.25, y: 0.25}));
// point(screen({x: 0.25, y: 0.25}));
// point(screen({x: 0.25, y: -0.25}));
// point(screen({x: -0.25, y: -0.25}));
