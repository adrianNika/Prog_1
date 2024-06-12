

import express from 'express';
import { createServer } from 'node:http';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { Server } from 'socket.io';

const app = express();
const server = createServer(app);
const io = new Server(server, {
  connectionStateRecovery: {}
});

const __dirname = dirname(fileURLToPath(import.meta.url));

// Configure Express pour servir les fichiers statiques
app.use(express.static(join(__dirname, '')));

app.get('/', (req, res) => {
  res.sendFile(join(__dirname, 'chat.html'));
});

io.on('connection', (socket) => {
  socket.on('chat message', (msg) => {
    io.emit('chat message', msg);
  });
});
server.listen(3000, () => {
  console.log('server running at http://localhost:3000');
});