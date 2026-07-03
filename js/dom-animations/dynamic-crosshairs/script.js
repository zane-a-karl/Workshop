
class dynamicCrossHairsAnimation {
    constructor() {
        // DOM objects
        this.crosshair = document.querySelector('.crosshair');
        this.outer_sight =
            document.querySelector('.outer-sight');
        this.north = document.querySelector('.north');
        this.south = document.querySelector('.south');
        this.east  = document.querySelector('.east');
        this.west  = document.querySelector('.west');

        // Animation context
        this.animation_id   = null;
        this.is_animtating  = false;
        this.is_resetting   = false;
        this.reset_progress = 0;
        this.time  = 0;
        this.DELTA = .8;//0.016; // ~60fps in seconds
        this.reset_duration = 0.8; // in seconds
        // even for forward; odd for backward
        this.click_pairity  = 0;

        // Debug flag - set to true to see console logs
        this.debug = true;

        // Initialize crosshairs context
        this.center_x = 0;
        this.center_y = 0;

        // Initialize mouse click event
        document.addEventListener('click', (event) => {
            if (this.debug) {
                console.log('Clicked element:', event.target);
                console.log('Mouse X:', event.clientX);
                console.log('Mouse Y:', event.clientY);
                console.log('Page X:', event.pageX);
                console.log('Page Y:', event.pageY);
                console.log('Ctrl key held:', event.ctrlKey);
                console.log('Shift key held:', event.shiftKey);
                 // 0 = left, 1 = middle, 2 = right
                console.log('Button:', event.button);
            }
            if (this.click_pairity % 2 === 0) {
                this.click_pairity = 1;
                this.center_x = event.clientX;
                this.center_y = event.clientY;
                this.animation_id =
                    requestAnimationFrame(() =>
                        this.forward_animation());
            } else {
                this.click_pairity = 0;
                this.animation_id =
                    requestAnimationFrame(() =>
                        this.backward_animation());
            }
        });
    }

    forward_animation() {
        if (this.debug) {
            console.log('=== ANIMATION STARTED ===');
        }
        this.place_sight();
        this.render_crosshairs();
        return;
    }

    place_sight() {
        // 1. render the outer_sight centered at the click
        this.outer_sight.style.display = "flex";
        const w = parseInt(
            getComputedStyle(this.outer_sight).width);
        const h = parseInt(
            getComputedStyle(this.outer_sight).height);
        this.outer_sight.style.left = `${this.center_x - w/2}px`;
        this.outer_sight.style.top = `${this.center_y - h/2}px`;
    }

    render_crosshairs() {
        // 0. display the hairs
        const vw = window.innerWidth;
        const vh = window.innerHeight;
        console.log(`vh = ${vh}`);
        console.log(`vw = ${vw}`);
        const crosshair_h = getComputedStyle(this.crosshair).height;
        this.crosshair.style.setProperty(`--center_y`, `${this.center_y}px`);
        this.crosshair.style.setProperty(`--center_x`, `${this.center_x}px`);
        const crosshair_w = getComputedStyle(this.crosshair).width;
        console.log(`crosshair_h = ${crosshair_h}`);
        console.log(`crosshair_w = ${crosshair_w}`);
        const crosshair_top = getComputedStyle(this.crosshair).top;
        const crosshair_left = getComputedStyle(this.crosshair).left;
        console.log(`crosshair_top = ${crosshair_top}`);
        console.log(`crosshair_left = ${crosshair_left}`);
        // 1. render north hair
        const nw = parseInt(
            getComputedStyle(this.north).width
        );
        // const zane = document.querySelector(".zane");
        // const start = `${vh - this.center_y}`; // 450;
        // zane.style.display = "flex";
        // zane.style.left = `${this.center_x - nw/2}px`;
        // zane.style.bottom = `${start}px`;
        const n_border_width = parseInt(
            getComputedStyle(this.north)
                .getPropertyValue('--border-width')
        );
        this.north.style.display = "flex";
        this.north.style.bottom = `${vh - this.center_y}px`;
        // this.north.style.bottom = `${vh - start}px`;
        this.north.style.left = `${this.center_x - nw/2}px`;
        this.north.style.setProperty(`--arrow-left`, `${-(n_border_width - nw/2)}px`);
        this.north.style.setProperty(`--arrow-top`, `${-n_border_width}px`);

        // 2. render south hair
        const sw = parseInt(getComputedStyle(this.south).width);
        const s_border_width = parseInt(getComputedStyle(this.south).getPropertyValue('--border-width'));
        this.south.style.display = "flex";
        this.south.style.top = `${this.center_y}px`;
        this.south.style.left = `${this.center_x - sw/2}px`;
        this.south.style.setProperty(`--arrow-left`, `${-(s_border_width - sw/2)}px`);
        this.south.style.setProperty(`--arrow-bottom`, `${-s_border_width}px`);

        // 3. render east hair
        const eh = parseInt(getComputedStyle(this.east).height);
        const e_border_width = parseInt(getComputedStyle(this.east).getPropertyValue('--border-width'));
        this.east.style.display = "flex";
        this.east.style.top = `${this.center_y - eh/2}px`;
        this.east.style.left = `${this.center_x}px`;
        this.east.style.setProperty(`--arrow-top`, `${-(e_border_width - eh/2)}px`);
        this.east.style.setProperty(`--arrow-right`, `${-e_border_width}px`);

        // 4. render west hair
        const wh = parseInt(getComputedStyle(this.west).height);
        const w_border_width = parseInt(getComputedStyle(this.west).getPropertyValue('--border-width'));
        this.west.style.display = "flex";
        this.west.style.top = `${this.center_y - wh/2}px`;
        this.west.style.right = `${vw - this.center_x}px`;
        this.west.style.setProperty(`--arrow-top`, `${-(w_border_width - wh/2)}px`);
        this.west.style.setProperty(`--arrow-left`, `${-w_border_width}px`);

    }

    backward_animation() {
        this.clear_screen();
        if (this.debug) {
            console.log('=== RESET COMPLETE ===');
        }
        return;
    }

    clear_screen() {
        // Just wipe everything
        cancelAnimationFrame(this.animation_id);
        this.north.style.display = "none";
        this.south.style.display = "none";
        this.west.style.display = "none";
        this.east.style.display = "none";
        this.outer_sight.style.display = "none";
        // Future animation
        // 1. ease retract north hair
        // 2. ease retract south hair
        // 3. ease retract east hair
        // 4. ease retract west hair
        // 5. when they reach the sight remove the sight
        // 6. play a popping sound
    }

    // ease out cubic for smooth deceleration
    ease_out_cubic(t) {
        return 1 - Math.pow(1 - t, 3);
    }
}

// Wait for DOM to be ready
document.addEventListener('DOMContentLoaded', () => {
    new dynamicCrossHairsAnimation(
    );
});
