const DELTA = .8;//0.016; // ~60fps in seconds

class ZKLogoAnimation {
    constructor(radius, speed, phaseOffset) {
        // DOM objects
        this.test = document.querySelector('.test');
        this.logoSection = document.querySelector('.logo-section');
        this.zkText = document.querySelector('.logo-text');
        this.morseElements =
            document.querySelectorAll('.morse-dot, .morse-dash');

        // Animation context
        this.animationId = null;
        this.isAnimating = false;
        this.isResetting = false;
        this.resetProgress = 0;
        this.time = 0;

        // Debug flag - set to true to see console logs
        this.debug = true;

        // Configure point of rotation
        const rect = this.zkText.getBoundingClientRect();
        this.centerX = rect.left + (rect.width / 2);
        this.centerY = rect.top + (rect.height / 2);
        this.centerZ = 0; // zkText is flat on the screen
        this.axisOfRotation = "z";

        // Save original element positions
        this.originalPositions = [];
        this.saveOriginalPositions(this.centerX, this.centerY, this.centerZ);
        console.log(`${JSON.stringify(this.originalPositions, null, 2)}`);

        if (this.debug) {
            console.log('=== INITIALIZATION ===');
            console.log('Center point:', {x: this.centerX, y: this.centerY});
            console.log('ZK Text rect:', rect);
        }

        // Configure animation
        this.radius = radius;
        this.speed = speed;
        this.phaseOffset = phaseOffset;

        // Configure waviness of circle
        this.amplitude = this.radius / 10;
        this.frequency = .100; // waves per circle

        // Animation configuration
        this.resetDuration = 0.8; // in seconds
        // Make test circle visible at center point
        this.updateTestCircle(this.centerX, this.centerY);

        this.init();
    }

    saveOriginalPositions(centerX, centerY, centerZ) {
        this.morseElements.forEach((element) => {
            // Get computed position
            const rect = element.getBoundingClientRect();
            // this.originalPositions.push({
            //     x: 0, // Relative to transform origin
            //     y: 0,
            //     rotation: 0,
            //     element: element
            // });
            this.originalPositions.push({
                x: 0, // Relative to transform origin
                y: 0,
                z: 0,
                element: element,
                offsetX: (rect.left + rect.width / 2) - centerX,
                offsetY: (rect.top + rect.height / 2) - centerY,
                offsetZ: centerZ,
            });
        });

        if (this.debug) {
            console.log('Saved original positions for', this.originalPositions.length, 'elements');
            console.log('Original offsets:', this.originalPositions.map(p => ({x: p.offsetX, y: p.offsetY})));
        }
    }

    updateTestCircle(x, y) {
        // FIX: Use style.left and style.top, not direct properties
        this.test.style.left = `${x}px`;
        this.test.style.top = `${y}px`;

        if (this.debug) {
            console.log('Test circle positioned at:', {x, y});
        }
    }

    init() {
        this.logoSection.addEventListener(
            'mouseenter', this.startAnimation.bind(this));
        this.logoSection.addEventListener(
            'mouseleave', this.resetAnimation.bind(this));

        if (this.debug) {
            console.log('Event listeners attached');
        }

        this.morseElements.forEach((element) => {
            const rect = element.getBoundingClientRect();
            const offsetX = this.centerX - (rect.left + rect.width / 2);
            const offsetY = this.centerY - (rect.top + rect.height / 2);

            element.style.transformOrigin = `${offsetX}px ${offsetY}px`;
        });
    }

    startAnimation() {
        if (this.isAnimating || this.isResetting) {
            return;
        }

        if (this.debug) {
            console.log('=== ANIMATION STARTED ===');
        }

        this.isAnimating = true;
        this.time = 0; // Reset time when starting
        this.update();
    }

    update() {
        if (this.isResetting) {
            this.resetProgress += DELTA / this.resetDuration;

            if (this.resetProgress >= 1) {
                // Reset complete
                this.isResetting = false;
                this.resetProgress = 0;
                this.morseElements.forEach((element, index) => {
                    this.resetMorseElement(
                        element,
                        this.originalPositions[index],
                        1);
                });

                if (this.debug) {
                    console.log('=== RESET COMPLETE ===');
                }

                return;
            }

            // Animate back to original positions
            this.morseElements.forEach(
                (element, index) => {
                    this.resetMorseElement(element,
                                            this.originalPositions[index],
                                            this.resetProgress);
            });

            this.animationId = requestAnimationFrame(() => this.update());
            return;
        }

        if (!this.isAnimating) {
            return;
        }

        // Apply animation logic to each element
        // this.zkText.style.transform = `rotateZ(${45}deg)`;
        this.morseElements.forEach((element, index) => {
            const position = this.updateElement(element, DELTA, index);
            this.applyCircularMotion(element, position, index, this.axisOfRotation);
            // this.applyLayeringAnimation(element, position, index, this.time);
        });

        // Schedule next frame
        this.animationId = requestAnimationFrame(() => this.update());
    }

    resetMorseElement(element, originalPos, progress) {
        // Get current transform values
        const transform = element.style.transform;

        // Parse current position (simplified - assumes translate3d exists)
        let currentX = 0;
        let currentY = 0;
        // let currentRotation = 0;

        if (transform) {
            const translate3dMatch = transform.match(/translate3d\(([^,]+)px,\s*([^,]+)px/);
            // const rotateMatch = transform.match(/rotateZ\(([^)]+)deg\)/);

            if (translate3dMatch) {
                currentX = parseFloat(translate3dMatch[1]);
                currentY = parseFloat(translate3dMatch[2]);
            }
            // if (rotateMatch) {
            //     currentRotation = parseFloat(rotateMatch[1]);
            // }
        }

        // Apply easing to progress
        const easedProgress = this.easeOutCubic(progress);

        // Interpolate between current and original
        const x = currentX + (originalPos.x - currentX) * easedProgress;
        const y = currentY + (originalPos.y - currentY) * easedProgress;
        // const rotation = currentRotation + (originalPos.rotation - currentRotation) * easedProgress;

        // Apply the interpolated position
        // element.style.transform =
        //     `translate3d(${x}px, ${y}px, 0px) rotateZ(${rotation}deg)`;
        element.style.transform = `translate3d(${x}px, ${y}px, 0px)`;
        element.style.zIndex = 100;
    }

    // Easing function - ease out cubic for smooth deceleration
    easeOutCubic(t) {
        return 1 - Math.pow(1 - t, 3);
    }

    updateElement(element, deltaTime, elementIndex) {
        this.time += deltaTime;

        // Add phase offset to place morse elements around circle
        const phase =
              (this.time * this.speed) + (elementIndex * this.phaseOffset);

        // Add sinusoidal overlay (waviness)
        const sineOffset =
              this.amplitude * Math.sin(this.frequency * this.time);
        const waveRadius = this.radius + sineOffset;

        // Basic circular motion
        const x = waveRadius * Math.cos(phase);
        const y = waveRadius * Math.sin(phase);

        // Update test circle to follow first element
        if (elementIndex === 0) {
            this.updateTestCircle(this.centerX + x, this.centerY + y);
        }

        // Add rotation
        // const rotation = phase * (180 / Math.PI);

        // this.addLayeringAnimation(element, elementIndex, {
        //     x: x + sineOffset,
        //     y: y + sineOffset,
        //     rotation: rotation
        // }, this.time);

        // return {
        //     x: x + sineOffset,
        //     y: y + sineOffset,
        //     rotation: rotation
        // };
        return {
            x: x + sineOffset,
            y: y + sineOffset
        };
    }

    // apply circular motion around chosen axis
    applyCircularMotion(element, position, elementIndex, axisOfRotation) {
        // Get this element's original offset from center
        const originalPos = this.originalPositions[elementIndex];
        switch (axisOfRotation) {
            // Circle flat on the screen/viewport
            case ("z"): {
                const x = position.x - originalPos.offsetX;
                const y = position.y - originalPos.offsetY;
                // Apply transform, accounting for where element started
                element.style.transform = `translate3d(${x}px, ${y}px, 0px)`;
                element.style.zIndex = 100;

                // Debug first element occasionally
                if (this.debug && elementIndex === 0 && Math.floor(this.time * 100) % 50 === 0) {
                    console.log('Element 0 - circlePos:', position, 'offset:', { x: originalPos.offsetX, y: originalPos.offsetY }, 'translate:', { x, y });
                }
            }
            break;
            // Circle like the rings of saturn
            case ("y"): {
                const x = position.x - originalPos.offsetX;
                const z = position.y - originalPos.offsetZ; // Use position.y for z here to change axes
                // Apply transform, accounting for where element started
                element.style.transform = `translate3d(${x}px, 0px, ${z}px)`;
                element.style.zIndex = z;

                // Debug first element occasionally
                if (this.debug && elementIndex === 0 && Math.floor(this.time * 100) % 50 === 0) {
                    console.log('Element 0 - circlePos:', position, 'offset:', { x: originalPos.offsetX, z: originalPos.offsetZ }, 'translate:', { x, z });
                }
            }
            break;
            // Circle like the rings of uranus
            case ("x"): {
                const y = position.y - originalPos.offsetY;
                const z = position.x - originalPos.offsetZ; // Use position.x for z here to change axes
                // Apply transform, accounting for where element started
                element.style.transform = `translate3d(0px, ${y}px, ${z}px)`;
                element.style.zIndex = z;

                // Debug first element occasionally
                if (this.debug && elementIndex === 0 && Math.floor(this.time * 100) % 50 === 0) {
                    console.log('Element 0 - circlePos:', position, 'offset:', { y: originalPos.offsetY, z: originalPos.offsetZ }, 'translate:', { y, z });
                }
            }
            break;
        }
    }

    applyLayeringAnimation(element, elementIndex, position, time) {
        // Calculate z-depth based on position
        const zDepth =
              Math.sin(time * 2 + elementIndex * this.phaseOffset) * 10;

        element.style.transform =
            `translate3d(${position.x}px, ${position.y}px, ${zDepth}px)
             rotateZ(${position.rotation}deg)`;

        // Update z-index based on z-depth
        element.style.zIndex = Math.round(zDepth + 100);
    }

    resetAnimation() {
        if (this.debug) {
            console.log('=== RESET TRIGGERED ===');
        }
        this.isAnimating = false;
        this.isResetting = true;
        this.resetProgress = 0;

        // Continue the animation loop for reset
        if (!this.animationId) {
            this.update();
        }
    }
}

// Causes them to spin around a different center for each
// Wait for DOM to be ready
document.addEventListener('DOMContentLoaded', () => {
    new ZKLogoAnimation(
        80,   // radius - how big the circle is
        0.009, // speed of animation
        0.9   // phaseOffset - spacing between elements
    );
});
