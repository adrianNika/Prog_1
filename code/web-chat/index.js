import express from 'express';
import { createServer } from 'node:http';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { Server } from 'socket.io';
import mongoose from 'mongoose';

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


mongoose.connect('mongodb+srv://Boa:12345@cluster0.jl9sfks.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0',
{ 
  useNewUrlParser: true,
  useUnifiedTopology: true 
})
  .then(() => console.log('Connexion à MongoDB réussie !'))
  .catch(() => console.log('Connexion à MongoDB échouée !'));


server.listen(3000, () => {
  console.log('server running at http://localhost:3000');
});