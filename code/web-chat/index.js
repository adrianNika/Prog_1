import express from 'express';
import { createServer } from 'node:http';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { Server } from 'socket.io';
import { MongoClient, ServerApiVersion } from 'mongodb';
import bcrypt from 'bcrypt';


const app = express();
const port = 3000;
const server = createServer(app);
const io = new Server(server);

const __dirname = dirname(fileURLToPath(import.meta.url));

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Configure Express to serve static files
app.use(express.static(join(__dirname, '')));

app.get('/', (req, res) => {
  res.sendFile(join(__dirname, 'login.html'));
});

const uri = "mongodb+srv://Boa:12345@cluster0.jl9sfks.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";
// Create a MongoClient with a MongoClientOptions object to set the Stable API version
const client = new MongoClient(uri, {
  serverApi: {
    version: ServerApiVersion.v1,
    strict: true,
    deprecationErrors: true,
  }
});


async function run() {
  try {
    await client.connect();
    await client.db("admin").command({ ping: 1 });

    const db = client.db('chatDB');
    const userCollections = db.collection('users');

    app.post('/register', async (req, res) => { 
      const {username, email, telephone, password} = req.body;
      const hashedPassword = await bcrypt.hash(password, 10);
      const userData = {username, email, telephone, password : hashedPassword};

      try 
      {
        const result = await userCollections.insertOne(userData);
        res.status(201).send('Inscription réussie ma poule');
      } catch (error) 
      {
        res.status(500).send('Inscription non réussie ma poule');
      }
    });

    app.post('/login', async (req, res) => { 
      const {username, password} = req.body;
      const hashedPassword = await bcrypt.hash(password, 10);

      try
      {
          const user = await userCollections.findOne({username})
          if (!user) 
          {
              return res.status(400).send('Le username est faux ou le compte n\'existe pas');
          }
          else
          {
              const isPasswordValid = await bcrypt.compare(password, user.password);
    
              if (!isPasswordValid) {
                return res.status(400).send('Mot de passe incorrect');
              }
            
            res.status(200).json(user);
          } 
      }
      catch (error) 
      {
          res.status(400).send('connexion échoué ma poule');
      }
    });

    server.listen(3001, () => {
      console.log('server running at http://localhost:3001');
    });

    console.log("Pinged your deployment. You successfully connected to MongoDB!");
  } catch (err) {
    console.error('Erreur de connexion à MongoDB:', err);
  }
}


let users = [];

io.on('connection', (socket) => {
  socket.on('chat message', (msg) => {
    io.emit('chat message', msg);
  });

  socket.on('register', (data) => {
    socket.username = data.userUsername;
    users.push(socket.username);
    io.emit('update list', users);
  });
});

run().catch(err => {
  console.error('Échec de la connexion à MongoDB:', err);
  process.exit(1); // Quitter le processus en cas d'échec de connexion
});