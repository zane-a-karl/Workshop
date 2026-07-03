var button = document.querySelector("button");
var body = document.querySelector("body");
var hex_value = document.getElementById("hexvalue");

body.style.backgroundColor = "gray";
var total_hex = 16*16-1;
button.onclick = function() {
    let hex = "#" +
	Math.floor(Math.random()*total_hex).toString(16) +
	Math.floor(Math.random()*total_hex).toString(16) +
	Math.floor(Math.random()*total_hex).toString(16);
    body.style.backgroundColor = hex;
    hex_value.textContent = hex;
}
