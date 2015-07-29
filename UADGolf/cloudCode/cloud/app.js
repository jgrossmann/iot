express = require('express');
app = express();

// Global app configuration section
app.set('views', 'cloud/views');  // Specify the folder to find templates
app.set('view engine', 'jade');    // Set the template engine
app.use(express.bodyParser());    // Middleware for reading request body

var views = require('cloud/views.js');
app.get('/', views.index);
app.get('/home', views.index);
app.get('/#', views.index);
app.get('/documentation', views.documentation);
app.get('/sampleputt', views.sampleputt);
app.get('/documentation/detailed_description', views.detailedDescription);
app.get('/documentation/abstract', views.abstract);
app.get('/documentation/overview', views.overview);
app.get('/documentation/resources', views.resources);
app.get('/documentation/references', views.references);
app.get('/documentation/software', views.software);
app.get('/documentation/stateoftheart', views.stateoftheart);
app.get('/documentation/results', views.results);
app.get('/documentation/futurework', views.futurework);
app.get('/documentation/conclusion', views.conclusion);


app.listen();
