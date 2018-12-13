const express = require('express');

const app = express();
app.set('view engine', 'ejs');

const enters = {}, leaves = {};
// setup storage
for (let i = 0; i < 24; i++) {
  enters[i] = leaves[i] = 0;
}

enters[1] = 21;
enters[2] = 22;
enters[3] = 20;

leaves[4] = 5;
leaves[8] = 9;
leaves[7] = 2;

app.get('/', (req, res) => {
  res.send('Hello');
});

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
  const ts = { req };
  console.log(ts);
  const hour = parseHour(ts);
  enters[hour] += 1;
  res.send('ok');
});

app.post('/leave', (req, res) => {
  const ts = { req } ;
  console.log(ts);
  const hour = parseHour(ts);
  leaves[hour] += 1;
  res.send('ok');
});

function parseHour(ts) {
  return ts;
}

app.listen(3000, () => console.log('Running on port 3000!'));