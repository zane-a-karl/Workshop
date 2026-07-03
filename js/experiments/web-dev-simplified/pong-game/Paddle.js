const CPU_SPEED = 0.02;

export default class Paddle {
		constructor(paddleElem) {
				this.paddleElem = paddleElem;
				this.reset();
		}

		get position() {
				// take css # of 50 for --postion and convert into js
				// usable value.
				return parseFloat(getComputedStyle(this.paddleElem).getPropertyValue("--position"));
		}

		set position(value) {
				this.paddleElem.style.setProperty("--position", value);
		}

		rect() {
				return this.paddleElem.getBoundingClientRect();
		}

		reset() {
				this.position = 50;
		}

		update(delta, ballHeight) {
				this.position += CPU_SPEED * delta * (ballHeight - this.position);
		}
}
