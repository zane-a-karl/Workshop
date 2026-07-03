var slider = document.querySelector(".slider");

var output = document.querySelector(".demo");
output.innerHTML = slider.value;
var body = document.querySelector("body");

slider.oninput = function() {
    let v = this.value;
    let step = 255;
    output.innerHTML = v;
    changeHex(v);
}

function rgbToHex(r, g, b) {
    return "#" + r.toString(16) + g.toString(16) + b.toString(16);
}

// https://www.w3schools.com/css/css_colors_hex.asp is super helpful.
function changeHex(v) {
    var r, g, b;
    let step = 255;
    let phase = Math.floor(v/step);
    switch (phase) {
    case 0:
	r = step;
	g = (v % step);
	b = 0;
	break;
    case 1:
	r = step - (v % step);
	g = step;
	b = 0;
	break;
    case 2:
	r = 0;
	g = step;
	b = (v % step);
	break;
    case 3:
	r = 0;
	g = step - (v % step);
	b = step;
	break;
    default:
	r = step;
	g = step;
	b = step;
	break;
    }

    body.style.background = "rgb(" + r + "," + g + "," + b + ")";
    document.querySelector('.hexresulttext').innerHTML = rgbToHex(r, g, b);
    if ( b == 255 ) {
	document.querySelector('.hexresulttext').style.color = "#ffffff";
    } else {
	document.querySelector('.hexresulttext').style.color = "#1f2d3d";
    }
}
