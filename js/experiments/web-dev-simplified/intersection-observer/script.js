const cards = document.querySelectorAll(".card");

const observer = new IntersectionObserver(
    entries => {
	entries.forEach(entry => {
	    // If we are intersecting "entry.isIntersecting" will
	    // be true and we then toggle the show class ON
	    // otherwise "entry.isIntersecting" will be false and
	    // we toggle it OFF.
	    // NOTE: that we will get a TRUE as soon as 1st px
	    // of the element comes upon the screen which will
	    // wreck our animation timing. We want to wait
	    // until the entire element is visible on the screen.
	    entry.target.classList.toggle("show", entry.isIntersecting);
	    // Prohibit page from reanimating previously animated
	    // cards by removing them from the observer, for now
	    // comment out because we like the reanimation
	    // if (entry.isIntersecting) {
	    // 		observer.unobserve(entry.target);
	    // }
	});
    }, {
	// Allows you to define the container your element is in
	// Default is "document" but works with any
	// document.querySelector("<tag>")
	root: document,
	// This solves the problem in the NOTE.
	// Set to "0" by default
	// OBS: no ";" here, but ","
	threshold: 1,
	// "rootMargin" allows us to offset when action will
	// happen, by saying that our container is 100px less
	// than it normally would be. With neg number animation
	// occurs after the element is 100 px past its default
	// animation position. With pos num animation occurs when
	// element is 100 px from when it becomes on the screen.
	// Pos num's are useful when you want to preload images
	// Default is 0px
	rootMargin: "0px",
    });

// For lazy loading
const lastCardObserver = new IntersectionObserver(
    entries => {
	const lastCard = entries[0];
	if (!lastCard.isIntersecting) {
	    return
	}
	// Fn only run if our last card is visible
	loadNewCards();
	// remove old "last card"
	lastCardObserver.unobserve(lastCard.target);
	// add new "last card"
	lastCardObserver.observe(document.querySelector(".card:last-child"));
    },
    {
	// Start the creation/loading of new cards early
	rootMargin: "100px",
    }
);

lastCardObserver.observe(document.querySelector(".card:last-child"));

// Observes every card in our list, and observe only takes one
// element at a time so we need to loop through the cards
cards.forEach(card => {
    observer.observe(card);
});

const cardContainer = document.querySelector(".card-container");

// Imagine we're doing a fetch to some API to get the new cards
// Here we just add them manually.
function loadNewCards() {
    for (let i = 0; i < 10; i++) {
	const card = document.createElement("div");
	card.textContent = "New Card";
	card.classList.add("card");
	observer.observe(card);
	cardContainer.append(card);
    }
}
