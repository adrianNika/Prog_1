const mongoose = require('mongoose');

const thingSchema = mongoose.Schema({
  username: { type: String, required: true },
  email: { type: String, required: true },
  telephone: { type: Number, required: true },
  password: { type: String, required: true },
});

module.exports = mongoose.model('Thing', thingSchema);