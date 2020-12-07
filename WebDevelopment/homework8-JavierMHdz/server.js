const XMLHttpRequest = require('xmlhttprequest').XMLHttpRequest;
const node_express = require('express');
//Initialize a server
const app = node_express();
//Cross Origin Research Sharing: allow sharing host
const cors = require('cors');
const port = process.env.PORT||5000;
//Use CORS
app.use(cors());

/************** The Guardian **************/

app.get('/search/guardian',(req,res) => {
    //Request is here
    const url = "https://content.guardianapis.com/search?api-key=31844137-b096-401d-8a85-d4788f81ca57&section=(sport|business|technology|politics)&show-blocks=all";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                console.log(obj);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();
    
})

/* The Guardian with Section Parameter */ 
app.get('/search/guardian/:parameter',(req,res) => {
    //Request is here
    const url = "https://content.guardianapis.com/" + req.params.parameter + "?api-key=31844137-b096-401d-8a85-d4788f81ca57&show-blocks=all";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                console.log(obj);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();

})

/* The Guardian with Article ID (Detailed Articles) */
app.get('/search/guardian/detailed/:id',(req,res) => {
    //Request is here
    const url = "https://content.guardianapis.com/" + req.params.id + "?api-key=31844137-b096-401d-8a85-d4788f81ca57&show-blocks=all";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                console.log(obj);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();

})

/* The Guardian with Keyword */
app.get('/search/guardian/keyword/:search_word',(req,res) => {
    //Request is here
    const url = "https://content.guardianapis.com/search?q=" + req.params.search_word + "&api-key=31844137-b096-401d-8a85-d4788f81ca57&show-blocks=all";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                console.log(obj);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();

})

/************** New York Times **************/
app.get('/search/nytimes',(req,res) => {
    //Request is here
    const url = "https://api.nytimes.com/svc/topstories/v2/home.json?api-key=uWTxI6S0NHGTOAB8xGVwjUd4ejcsrVEK";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();
    
})

app.get('/search/nytimes/:parameter',(req,res) => {
    //Request is here
    const url = "https://api.nytimes.com/svc/topstories/v2/" + req.params.parameter + ".json?api-key=uWTxI6S0NHGTOAB8xGVwjUd4ejcsrVEK";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();
})

app.get('/search/nytimes/detailed/:id',(req,res) => {
	//Request is here
	const url = 'https://api.nytimes.com/svc/search/v2/articlesearch.json?fq=web_url:("' + req.params.id + '")&api-key=uWTxI6S0NHGTOAB8xGVwjUd4ejcsrVEK';
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();
})

app.get('/search/nytimes/keyword/:search_word',(req,res) => {
    //Request is here
    const url = "https://api.nytimes.com/svc/search/v2/articlesearch.json?q=" + req.params.search_word + "&api-key=uWTxI6S0NHGTOAB8xGVwjUd4ejcsrVEK";
    const xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
                res.send(obj)
	        }
	    }
	}
	xmlhttp.send();
})

app.listen(port,function(){
    console.log("Server Started")
})

