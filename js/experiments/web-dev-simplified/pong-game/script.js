import Ball from './Ball.js';
import Paddle from './Paddle.js';

const ball = new Ball (document.getElementById('ball'));
const playerPaddle = new Paddle(document.getElementById("player-paddle"));
const computerPaddle = new Paddle(document.getElementById("computer-paddle"));
const playerScoreElem = document.getElementById("player-score");
const computerScoreElem = document.getElementById("computer-score");

let lastTime;
function update (time) {

		if (lastTime != null) {
				// In order to control the update rate
				const delta = time - lastTime;
				// So that the ball updates change with delta
				ball.update(delta, [playerPaddle.rect(), computerPaddle.rect()]);
				computerPaddle.update(delta, ball.y);
				const hue = parseFloat(getComputedStyle(document.documentElement).getPropertyValue("--hue"));

				document.documentElement.style.setProperty("--hue", hue + delta * 0.01);

				if (isLose()) {
						handleLose();
				}
		}
		lastTime = time;
		window.requestAnimationFrame(update);
}

function isLose() {
		const rect = ball.rect();
		// Ball below screen left or above screen right
		return rect.right >= window.innerWidth || rect.left <= 0;
}

function handleLose() {
		const rect = ball.rect();
		if (rect.right >= window.innerWidth) {
				playerScoreElem.textContent = parseInt(playerScoreElem.textContent) + 1;
		} else {
				computerScoreElem.textContent = parseInt(computerScoreElem.textContent) + 1;
		}
		ball.reset();
		computerPaddle.reset(); 
}

// We don't need an update function for the player paddle because
// it is merely going to follow our mouse
document.addEventListener("mousemove", e => {
		// e.y is a pixel value so we need to convert it to a vh %age
		playerPaddle.position = (e.y / window.innerHeight) * 100;
});

// Don't use setInterval(update, 10) which claims to call 'update' every 10ms
// Use the following b/c setInterval isn't super accurate and may not actually run
// every 10ms. Also setInterval will run in b/n frames. The following calls 'update'
// only when something on the screen can be modified.
window.requestAnimationFrame(update);
