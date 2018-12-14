const express = require('express');

const app = express();
app.set('view engine', 'ejs');

const enters = {}, leaves = {};
// setup storage
for (let i = 0; i < 24; i++) {
  enters[i] = leaves[i] = 0;
}

// some pre-populated test data
enters[1] = 1;
enters[2] = 2;
enters[3] = 1;

leaves[4] = 1;
leaves[8] = 1;
leaves[7] = 2;

app.get('/', (_, res) => res.send('App is working!'));

app.get('/stats', (_, res) => {
  const mostOftenLeaveTime = Object.keys(leaves)
    .reduce((a, b) => leaves[a] > leaves[b] ? a : b);
  const mostOfterEnterTime = Object.keys(enters)
    .reduce((a, b) => enters[a] > enters[b] ? a : b);
  res.render('stats', {
    mostOftenLeaveTime, mostOfterEnterTime
  });
});

app.post('/enter', (req, res) => {
  const hour = req.ts;
  console.log(hour);
  enters[hour] += 1;
  res.send('ok');
});

app.post('/leave', (req, res) => {
  const hour = req.ts;
  console.log(hour);
  leaves[hour] += 1;
  res.send('ok');
});

app.listen(3000, () => console.log('Running on port 3000!'));