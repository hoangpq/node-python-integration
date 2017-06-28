const fs = require('fs');
const PORT = process.env.PORT || 3000;

// will block main thread
require('http').createServer((req, res) => {
  fs.readFile('./package.json', (err, data) => {
    res.writeHead(200, {
      'Content-Type': 'application/json',
    });
    res.end(data.toString());
  });
}).listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

// heap(memory)
// stack => function
// callback queue
