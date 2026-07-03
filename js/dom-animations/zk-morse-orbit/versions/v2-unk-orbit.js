class ZKLogoAnimation {
    constructor(centerX, centerY, radius, phaseOffset, amplitude, frequency) {
        // DOM objects
        this.logoSection = document.querySelector('.logo-section');
        this.morseElements =
            document.querySelectorAll('.morse-dot, .morse-dash');

        // Animation context
        this.animationId = null;
        this.isAnimating = false;
        this.isResetting = false;
        this.timeline = [];
        this.setupTimeLine();

        // Save original positions
        this.originalPositions = [];
        this.saveOriginalPositions();

        this.centerX = centerX;
        this.centerY = centerY;
        this.radius = radius;

        this.phaseOffset = phaseOffset;
        this.time = 0;

        this.amplitude = amplitude;
        this.frequency = frequency;

        // Reset animation state
        this.resetProgress = 0;
        this.resetDuration = 0.8; // seconds to reset

        this.init();
    }

    saveOriginalPositions() {
        this.morseElements.forEach((element) => {
            // Get computed position
            const rect = element.getBoundingClientRect();
            this.originalPositions.push({
                x: 0, // Relative to transform origin
                y: 0,
                rotation: 0,
                element: element
            });
        });
    }

    init() {
        this.logoSection.addEventListener(
            'mouseenter', this.startAnimation.bind(this));
        this.logoSection.addEventListener(
            'mouseleave', this.resetAnimation.bind(this));
    }

    // Easing function - ease out cubic for smooth deceleration
    easeOutCubic(t) {
        return 1 - Math.pow(1 - t, 3);
    }

    addLayeringAnimation(element, position, time) {
        // Calculate z-depth based on position
        const zDepth = Math.sin(time * 2) * 10;

        element.style.transform =
            `translate3d(${position.x}px, ${position.y}px, ${zDepth}px)
                   rotateZ(${position.rotation}deg)`;

        // Update z-index based on z-depth
        element.style.zIndex = Math.round(zDepth + 100);
    }

    updateElement(element, deltaTime, elementIndex) {
        this.time += deltaTime;

        // Add phase offset to base circular motion
        const phase = this.time + (elementIndex * this.phaseOffset);

        // Basic circular motion
        const x = this.centerX + this.radius * Math.cos(phase);
        const y = this.centerY + this.radius * Math.sin(phase);

        // Add sinusoidal overlay
        const sineOffset =
              this.amplitude * Math.sin(this.time * this.frequency);

        // Add rotation
        const rotation = phase * (180 / Math.PI) % 360;

        this.addLayeringAnimation(element, { 
            x: x * sineOffset, 
            y: y * sineOffset,
            rotation: rotation
        }, this.time);

        return {
            x: x * sineOffset,
            y: y * sineOffset,
            rotation: rotation
        };
    }

    resetElementToOriginal(element, originalPos, progress) {
        // Get current transform values
        const transform = element.style.transform;
        
        // Parse current position (simplified - assumes translate3d exists)
        let currentX = 0, currentY = 0, currentRotation = 0;
        
        if (transform) {
            const translate3dMatch = transform.match(/translate3d\(([^,]+)px,\s*([^,]+)px/);
            const rotateMatch = transform.match(/rotateZ\(([^)]+)deg\)/);
            
            if (translate3dMatch) {
                currentX = parseFloat(translate3dMatch[1]);
                currentY = parseFloat(translate3dMatch[2]);
            }
            if (rotateMatch) {
                currentRotation = parseFloat(rotateMatch[1]);
            }
        }

        // Apply easing to progress
        const easedProgress = this.easeOutCubic(progress);

        // Interpolate between current and original
        const x = currentX + (originalPos.x - currentX) * easedProgress;
        const y = currentY + (originalPos.y - currentY) * easedProgress;
        const rotation = currentRotation + (originalPos.rotation - currentRotation) * easedProgress;

        // Apply the interpolated position
        element.style.transform =
            `translate3d(${x}px, ${y}px, 0px) rotateZ(${rotation}deg)`;
        element.style.zIndex = 100;
    }

    update() {
        const DELTA = 1;//0.016; // ~60fps in seconds
        
        if (this.isResetting) {
            this.resetProgress += DELTA / this.resetDuration;
            
            if (this.resetProgress >= 1) {
                // Reset complete
                this.isResetting = false;
                this.resetProgress = 0;
                this.morseElements.forEach((element, index) => {
                    this.resetElementToOriginal(element, this.originalPositions[index], 1);
                });
                return;
            }

            // Animate back to original positions
            this.morseElements.forEach((element, index) => {
                this.resetElementToOriginal(element, this.originalPositions[index], this.resetProgress);
            });

            this.animationId = requestAnimationFrame(() => this.update());
            return;
        }

        if (!this.isAnimating) {
            return;
        }

        // Apply animation logic to each element
        this.morseElements.forEach((element, index) => {
            this.updateElement(element, DELTA, index);
        });

        // Schedule next frame
        this.animationId = requestAnimationFrame(() => this.update());
    }

    startAnimation() {
        if (this.isAnimating || this.isResetting) {
            return;
        }
        this.isAnimating = true;
        this.time = 0; // Reset time when starting
        this.update();
    }

    resetAnimation() {
        this.isAnimating = false;
        this.isResetting = true;
        this.resetProgress = 0;
        
        // Continue the animation loop for reset
        if (!this.animationId) {
            this.update();
        }
    }

    setupTimeLine() {
        this.morseElements.forEach((element, index) => {
            this.timeline.push({
                element: element,
                delay: index * 0.1, // Staggered start
                duration: 2,
                keyframes: this.generateKeyframes(index)
            });
        });
    }

    generateKeyframes(index) {
        const phase =
              (index * 2 * Math.PI) / this.morseElements.length;
        return [
            { time: 0, x: 0, y: 0, rotation: 0 },
            { time: 0.25, x: Math.cos(phase) * 50, y: Math.sin(phase) * 50, rotation: 90 },
            { time: 0.50, x: Math.cos(phase + Math.PI) * 50, y: Math.sin(phase + Math.PI) * 50, rotation: 180 },
            { time: 0.75, x: Math.cos(phase + 1.5 * Math.PI) * 50, y: Math.sin(phase + 1.5 * Math.PI) * 50, rotation: 270 },
            { time: 1.00, x: 0, y: 0, rotation: 360 },
        ];
    }
}

// Causes them to spin around a different center for each
// Wait for DOM to be ready
document.addEventListener('DOMContentLoaded', () => {
    const zk = new ZKLogoAnimation(
        0,      // centerX - center of circular motion
        0,      // centerY
        50,     // radius - how big the circle is
        0.5,    // phaseOffset - spacing between elements
        1,      // amplitude - wave intensity
        0.1     // frequency - wave speed
    );
});

// Causes them to spin with a tiny radius 
// document.addEventListener('DOMContentLoaded', () => {
//     const zk = new ZKLogoAnimation(
//         0,      // centerX (relative to element's position)
//         0,      // centerY
//         30,     // radius (smaller = tighter orbit)
//         Math.PI / 4,  // phaseOffset (spreads elements around circle)
//         0.5,    // amplitude (lower = less wave effect)
//         0.05    // frequency (lower = slower wave)
//     );
// });
