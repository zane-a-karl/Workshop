// Seems to be a goldielocks speed
const INITIAL_VELOCITY = 0.025;
const VELOCITY_INCREASE = 0.00001;

export default class Ball {
		constructor(ballElem) {
				this.ballElem = ballElem;
				this.reset();
		}

		get x() {
				// take css # of 50 for --x and convert into js usable value.
				return parseFloat(getComputedStyle(this.ballElem).getPropertyValue("--x"));
		}

		// Allows us to say "this.x = 5" and call this function
		set x(value) {
				this.ballElem.style.setProperty("--x", value);
		}

		get y() {
				// take css # of 50 for --y and convert into js usable value.
				return parseFloat(getComputedStyle(this.ballElem).getPropertyValue("--y"));
		}

		// Allows us to say "this.y = 5" and call this function
		set y(value) {
				this.ballElem.style.setProperty("--y", value);
		}

		// Returns the ball position...in a way
		rect() {
				return this.ballElem.getBoundingClientRect();
		}

		reset() {
				this.x = 50;
				this.y = 50;
				this.direction = { x: 0 };
				while (Math.abs(this.direction.x) <= .2 || Math.abs(this.direction.x) >= .9) {
						// Start the ball with a unit vec in a random direction
						const heading = randomNumberBetween(0, 2 * Math.PI);
						this.direction = { x: Math.cos(heading), y: Math.sin(heading) };
				}
				this.velocity = INITIAL_VELOCITY;
		}
		
		update(delta, paddleRects) {
				// We include the delta because if we have a large delay
				// b/n animations then we should have a correspondingly
				// large delay in magnitude of distance moved. I.e. to
				// scale with the frame update rate.
				this.x += this.direction.x * this.velocity * delta;
				this.y += this.direction.y * this.velocity * delta;
				this.velocity += VELOCITY_INCREASE * delta;
				const rect = this.rect();

				// Ball below screen bottom or above screen top
				if (rect.bottom >= window.innerHeight || rect.top <= 0) {
						this.direction.y *= -1;
				}
				// Ball below screen left or above screen right
				if (paddleRects.some(r => isCollision(r, rect))) {
						this.direction.x *= -1;
				}
		}
}

function randomNumberBetween(min, max) {
		return Math.random() * (max - min) + min;
}

function isCollision(rect1, rect2) {
		return rect1.left <= rect2.right &&
				rect1.right >= rect2.left &&
				rect1.top <= rect2.bottom &&
				rect1.bottom >= rect2.top;
}
