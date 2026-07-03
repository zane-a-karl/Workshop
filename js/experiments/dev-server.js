// dev-server.js
const express = require('express');
const path = require('path');
const app = express();
const PORT = 3000;

// Arguments after -- are in process.argv starting at index 2
const args = process.argv.slice(2);
console.log(`"npm run dev" executed with args: ${args}`);
const index_dir = args[0];
const serve_from_dir = path.join(__dirname, `${index_dir}`);
console.log(`serve_from_dir = ${serve_from_dir}`);

// Serve static files from root directory
app.use(express.static(serve_from_dir));

// Serve index.html for root route
app.get('/', (req, res) => {
    res.sendFile(path.join(serve_from_dir, "index.html"));
});

app.listen(PORT, () => {
    console.log(`Development server running at http://localhost:${PORT}`);
    console.log('Press Ctrl+C to stop');
});
