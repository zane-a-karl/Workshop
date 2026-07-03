var button = document.querySelector("button");
var body = document.querySelector("body");

body.style.backgroundColor = "gray";
button.onclick = function() {
    body.style.backgroundColor = NAMED_COLORS[Math.floor(Math.random() * NAMED_COLORS.length)];
}
