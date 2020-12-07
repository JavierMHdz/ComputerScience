window.onload = submitFunc('/headlines');

function submitFunc(url){
    deleteContent();
    var xmlhttp = new XMLHttpRequest();

	//GET = appends to URL, true = asynchronous
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            var obj = JSON.parse(xmlhttp.responseText);
	            var checkedTwoArrays = errorChecking(obj.articles);
	            var carouselArray = errorCheckingSingle(obj.carousel_articles);
                writeObj(checkedTwoArrays, carouselArray);
	            slideShow();
	            wordCloud(obj.word_cloud);
	            console.log(obj);
	        }
	    }
	}
	xmlhttp.send();
}

function addSources(url, category){
    var xmlhttp = new XMLHttpRequest();

    //GET = appends to URL, true = asynchronous
    xmlhttp.open("GET", url, true);

    xmlhttp.onreadystatechange = function(){
        if(xmlhttp.readyState == 4){
            if(xmlhttp.status == 200){
                var JSON_info = JSON.parse(xmlhttp.responseText);
                var sources = JSON_info.sources;
                if(category === null){
                    for(let i = 0; i < sources.length; i++){
                        const source_option = document.createElement("option");
                        source_option.innerHTML = sources[i].name;
                        source_option.value = sources[i].id;
                        document.getElementById("news_sources").appendChild(source_option);
                    }
                }
                else{
                    document.getElementById("news_sources").innerHTML = " ";
                    const all_sources = document.createElement("option");
                    all_sources.innerHTML = "All";
                    all_sources.value = " ";
                    all_sources.default = true;
                    document.getElementById("news_sources").appendChild(all_sources);
                    for(let i = 0; i < sources.length; i++){
                        if(category.localeCompare(sources[i].category) == 0){
                            const source_option = document.createElement("option");
                            source_option.innerHTML = sources[i].name;
                            source_option.value = sources[i].id;
                            document.getElementById("news_sources").appendChild(source_option);
                        }
                    }
                }
            }
        }
    }
    xmlhttp.send();
}

function writeObj(checkedTwoArrays, carouselArray){
    //deleteContent();
    var cnnArray = checkedTwoArrays[0];
	var foxArray = checkedTwoArrays[1];

    //Update active button
    document.getElementById("googleBtn").classList.add("activeBtn");
    document.getElementById("searchBtn").classList.remove("activeBtn");

	//Create header div
	const header = document.createElement("div");
	header.id = "header";
	document.getElementById("contentContainer").appendChild(header);

	//Create Carousel div
	const carouselDiv = document.createElement("div");
	carouselDiv.setAttribute("id","carouselDiv");
	header.appendChild(carouselDiv);

	//Create images inside Carousel div
    const max = 5;
	for(let i = 0; i < max; i++){
	    const changingDiv = document.createElement("div");
	    changingDiv.className = "slideshow";
	    const link = document.createElement("a");
	    link.href = carouselArray[i].url;
	    link.setAttribute("target","_blank");
	    changingDiv.appendChild(link);
        const imageTag = document.createElement("img");
        imageTag.setAttribute("src",carouselArray[i].urlToImage);

        link.appendChild(imageTag);

        const textDiv = document.createElement("div");
        textDiv.id = "textDiv";
        textDiv.innerHTML = "<span id='carouselTitle'><b>" + carouselArray[i].title + "</b></span><br><br><span id='carouselDescription'>" + carouselArray[i].description + "</span>";

        link.appendChild(textDiv);
        carouselDiv.appendChild(changingDiv);
	}

	//WordCloud Div
	const wordCloudDiv = document.createElement("div");
	wordCloudDiv.id = "wordCloudDiv";
	header.appendChild(wordCloudDiv);

    //Create CNN divs
    var cnnDiv = document.createElement("div");
    cnnDiv.setAttribute("id","cnnDiv");
    document.getElementById("contentContainer").appendChild(cnnDiv);

    //CNN Header
    var cnnHeader = document.createElement("h2");
    cnnHeader.innerHTML = "CNN";
    document.getElementById("cnnDiv").appendChild(cnnHeader);
    document.getElementById("cnnDiv").appendChild(document.createElement("hr"));

    //CNN0
    var cnn0 = document.createElement("div");
    cnn0.setAttribute("id", "cnn0");
    cnn0.setAttribute("class","cnnArt");
    var action = "window.open('" + cnnArray[0].url + "', '_blank')";
    cnn0.setAttribute("onclick",action);
    document.getElementById("cnnDiv").appendChild(cnn0);

    //CNN1
    var cnn1 = document.createElement("div");
    cnn1.setAttribute("id", "cnn1");
    cnn1.setAttribute("class","cnnArt");
    action = "window.open('" + cnnArray[1].url + "', '_blank')";
    cnn1.setAttribute("onclick",action);
    document.getElementById("cnnDiv").appendChild(cnn1);

    //CNN2
    var cnn2 = document.createElement("div");
    cnn2.setAttribute("id", "cnn2");
    cnn2.setAttribute("class","cnnArt");
    action = "window.open('" + cnnArray[2].url + "', '_blank')";
    cnn2.setAttribute("onclick",action);
    document.getElementById("cnnDiv").appendChild(cnn2);

    //CNN3
    var cnn3 = document.createElement("div");
    cnn3.setAttribute("id", "cnn3");
    cnn3.setAttribute("class","cnnArt");
    action = "window.open('" + cnnArray[3].url + "', '_blank')";
    cnn3.setAttribute("onclick",action);
    document.getElementById("cnnDiv").appendChild(cnn3);

    //cnnArray is checkedTwoArrays[0]
    for(var i = 0; i < 4; i++){
        var string = "cnn";
        string += i;
        document.getElementById(string).innerHTML = "<img src='" + cnnArray[i].urlToImage + "'><br><span><b>" +
        cnnArray[i].title + "</b><br><br>" + cnnArray[i].description + "</span>";
    }

    //Create Fox News divs
    var foxDiv = document.createElement("div");
    foxDiv.setAttribute("id","foxDiv");
    document.getElementById("contentContainer").appendChild(foxDiv);

    //Fox News Header
    var foxHeader = document.createElement("h2");
    foxHeader.innerHTML = "Fox News";
    document.getElementById("foxDiv").appendChild(foxHeader);
    document.getElementById("foxDiv").appendChild(document.createElement("hr"));

    //fox0
    var fox0 = document.createElement("div");
    fox0.setAttribute("id", "fox0");
    fox0.setAttribute("class","foxArt");
    action = "window.open('" + foxArray[0].url + "', '_blank')";
    fox0.setAttribute("onclick",action);
    document.getElementById("foxDiv").appendChild(fox0);

    //fox1
    var fox1 = document.createElement("div");
    fox1.setAttribute("id", "fox1");
    fox1.setAttribute("class","foxArt");
    action = "window.open('" + foxArray[1].url + "', '_blank')";
    fox1.setAttribute("onclick",action);
    document.getElementById("foxDiv").appendChild(fox1);

    //fox2
    var fox2 = document.createElement("div");
    fox2.setAttribute("id", "fox2");
    fox2.setAttribute("class","foxArt");
    action = "window.open('" + foxArray[2].url + "', '_blank')";
    fox2.setAttribute("onclick",action);
    document.getElementById("foxDiv").appendChild(fox2);

    //fox3
    var fox3 = document.createElement("div");
    fox3.setAttribute("id", "fox3");
    fox3.setAttribute("class","foxArt");
    action = "window.open('" + foxArray[3].url + "', '_blank')";
    fox3.setAttribute("onclick",action);
    document.getElementById("foxDiv").appendChild(fox3);

    //foxArray is checkedTwoArrays[1]
    for(var i = 0 ; i < 4; i++){
        var string = "fox";
        string += i;
        document.getElementById(string).innerHTML = "<img src='" + foxArray[i].urlToImage + "'><br><span><b>" +
        foxArray[i].title + "</b><br><br>" + foxArray[i].description + "</span>";
    }
}

function errorChecking(articles){
    var cnnArray = [];
    var foxArray = [];
    //var carouselArray = [];

    for (var i = 0; i < articles.length; i++){
        var article = articles[i];
        var author = article.author;
        var description = article.description;
        var title = article.title;
        var url = article.url;
        var urlToImage = article.urlToImage;
        var publishedAt = article.publishedAt;
        var source = article.source;

        if(author === null || description === null || description == "" || title === null || url === null ||
            urlToImage === null || publishedAt === null || source.id === null || source.name === null){
            continue;
        }

        if((source.id).localeCompare("cnn") == 0){
            cnnArray.push(article);
        }

        if((source.id).localeCompare("fox-news") == 0){
            foxArray.push(article);
        }

    }

    return [cnnArray,foxArray];
}

function deleteContent(){
    document.getElementById("contentContainer").innerHTML = "";
}


function createForm(){
    const formObj = document.createElement("form");
    formObj.id = "myForm";
    formObj.method = "get";

    formObj.appendChild(document.createElement("br"));
    formObj.appendChild(document.createElement("br"));

    const info_div = document.createElement("div");
    info_div.id = "where_info_go";
    formObj.appendChild(info_div);

    const keyword = document.createElement("label");
    keyword.for = "topic";
    keyword.innerHTML = "Keyword<span class='red'>*  </span>";
    info_div.appendChild(keyword);
    const topic = document.createElement("input");
    topic.id = "topic";
    topic.name = "topic";
    topic.type = "text";
    topic.required = "true";
    info_div.appendChild(topic);

    //Create current date
    const today = new Date();
    let day = today.getDate();
    let month = today.getMonth()+1;
    const year = today.getFullYear();
    if(day < 10){
        day = '0' + day;
    }

    if(month < 10){
        month = '0' + month;
    }

    const currentDate = year + "-" + month + "-" + day;

    //Create a week ago date
    let one_week_less = new Date();
    one_week_less.setDate(one_week_less.getDate() - 7);
    let past_day = one_week_less.getDate();
    let past_month = one_week_less.getMonth()+1;
    const past_year = one_week_less.getFullYear();
    if(past_day < 10){
        past_day = '0' + past_day;
    }

    if(past_month < 10){
        past_month = '0' + past_month;
    }

    const lastWeekDate = past_year + "-" + past_month + "-" + past_day;

    const from = document.createElement("label");
    from.for = "from_date";
    from.innerHTML = "  From<span class='red'>*  </span>";
    info_div.appendChild(from);
    const from_date = document.createElement("input");
    from_date.name = "from_date";
    from_date.id = "from_date_id";
    from_date.type = "date";
    from_date.required = "true";
    from_date.value = lastWeekDate;
    info_div.appendChild(from_date);

    const to = document.createElement("label");
    to.for = "to_date";
    to.innerHTML = "  To<span class='red'>*  </span>"
    info_div.appendChild(to);
    const to_date = document.createElement("input");
    to_date.name = "to_date";
    to_date.id = "to_date_id";
    to_date.type = "date";
    to_date.required = "true";
    to_date.value = currentDate;
    info_div.appendChild(to_date);

    formObj.appendChild(document.createElement("br"));

    const sources_div = document.createElement("div");
    sources_div.id = "where_selects_go";
    formObj.appendChild(sources_div);

    const category = document.createElement("label");
    category.for = "news_category";
    category.innerHTML = "Category";
    sources_div.appendChild(category);

    const categorySelect = document.createElement("select");
    categorySelect.name = "news_category";
    categorySelect.id = "news_category";
    categorySelect.onchange = () => addSources('/sources', categorySelect.value);
    sources_div.appendChild(categorySelect);

    const allOption = document.createElement("option");
    allOption.innerHTML = "All";
    allOption.value = " ";
    allOption.default = true;
    categorySelect.appendChild(allOption);

    const businessOption = document.createElement("option");
    businessOption.innerHTML = "Business";
    businessOption.value = "business";
    categorySelect.appendChild(businessOption);

    const entertainmentOption = document.createElement("option");
    entertainmentOption.innerHTML = "Entertainment";
    entertainmentOption.value = "entertainment";
    categorySelect.appendChild(entertainmentOption);

    const generalOption = document.createElement("option");
    generalOption.innerHTML = "General";
    generalOption.value = "general";
    categorySelect.appendChild(generalOption);

    const healthOption = document.createElement("option");
    healthOption.innerHTML = "Health";
    healthOption.value = "health";
    categorySelect.appendChild(healthOption);

    const scienceOption = document.createElement("option");
    scienceOption.innerHTML = "Science";
    scienceOption.value = "science";
    categorySelect.appendChild(scienceOption);

    const sportsOption = document.createElement("option");
    sportsOption.innerHTML = "Sports";
    sportsOption.value = "sports";
    categorySelect.appendChild(sportsOption);

    const technologyOption = document.createElement("option");
    technologyOption.innerHTML = "Technology";
    technologyOption.value = "technology";
    categorySelect.appendChild(technologyOption);

    /*Label*/
    const label_sources = document.createElement("label");
    label_sources.for = "news_sources";
    label_sources.innerHTML = "Source"
    sources_div.appendChild(label_sources);

    /*Select*/
    const select_sources = document.createElement("select");
    select_sources.name = "news_sources";
    select_sources.id = "news_sources";
    sources_div.appendChild(select_sources);
    const allOptionTwo = document.createElement("option");
    allOptionTwo.innerHTML = "All";
    allOptionTwo.value = " ";
    allOptionTwo.default = true;
    select_sources.appendChild(allOptionTwo);

    formObj.appendChild(document.createElement("br"));

    const btn_div = document.createElement("div");
    btn_div.id = "where_btn_go";
    formObj.appendChild(btn_div);

    const searchBtn = document.createElement("input");
    searchBtn.type = "submit";
    searchBtn.id = "search_for_form";
    searchBtn.value = "Search";
    btn_div.appendChild(searchBtn);

    const clearBtn = document.createElement("input");
    clearBtn.type = "button";
    clearBtn.value = "Clear";
    clearBtn.id = "clear";
    btn_div.appendChild(clearBtn);

    addSources('/sources', null);

    return formObj;
}

function handleSearch(){
    deleteContent();
    clearTimeout(window.carousel);

    //Update active button
    document.getElementById("googleBtn").classList.remove("activeBtn");
    document.getElementById("searchBtn").classList.add("activeBtn");

    var newForm = createForm();

    const formDiv = document.createElement("div");
    formDiv.id = "formDiv";
    const queryDiv = document.createElement("div");
    queryDiv.id = "queryDiv";

    document.getElementById("contentContainer").appendChild(formDiv);
    document.getElementById("contentContainer").appendChild(queryDiv);

    formDiv.appendChild(newForm);
    document.getElementById("myForm").addEventListener("submit",(event) =>{

        event.preventDefault();

        const date_one = document.getElementById("from_date_id").value;

        const date_two = document.getElementById("to_date_id").value;

        const startDate = new Date(date_one);
        const endDate = new Date(date_two);

        if(startDate > endDate){
            alert("Incorrect Time");
        }
        else{
            callFunc(newForm);
        }

    });
    document.getElementById("clear").addEventListener("click",clearForm);
}

function clearForm(){
    document.getElementById("topic").value = "";
    document.getElementById("queryDiv").innerHTML = " ";
}

document.getElementById("searchBtn").addEventListener("click",handleSearch);

function slideShow(){
    var counter = 0;
    carouselShow();

    function carouselShow(){
        var x = document.getElementsByClassName("slideshow");
        for (var i = 0; i < x.length; i++){
            x[i].style.display = "none";
        }
        counter++;
        if(counter > x.length){
            counter = 1;
        }
        x[counter-1].style.display = "block";
        window.carousel = setTimeout(carouselShow, 3000);
    }
}

function errorCheckingSingle(articles){
    var finalArray = [];

    for (var i = 0; i < articles.length; i++){
        var article = articles[i];
        var author = article.author;
        var description = article.description;
        var title = article.title;
        var url = article.url;
        var urlToImage = article.urlToImage;
        var publishedAt = article.publishedAt;
        var source = article.source;

        if(author === null || description === null || description == "" || title === null || url === null ||
            urlToImage === null || publishedAt === null || source.id === null || source.name === null){
            continue;
        }

        if(finalArray.length != 15){
            finalArray.push(article);
        }

    }

    return finalArray;
}

function wordCloud(myWords){
    //set the dimensions and margins graph
    const margin = {top: 10, right: 10, bottom: 10, left: 10},
          width= 280 - margin.left - margin.right,
          height= 280 - margin.top - margin.bottom;

    //append the svg object to the body of the page
    const svg = d3.select("#wordCloudDiv").append("svg")
          .attr("width", width + margin.left + margin.right)
          .attr("height", height + margin.top + margin.bottom)
        .append("g")
          .attr("transform",
                "translate(" + margin.left + "," + margin.top + ")");

    //Constructs a new cloud layout instance. It run an algorithm to find the position of words that suits your requirements
    //wordCloud features that are different from one word to the other must be here
    const layout = d3.layout.cloud()
        .size([width, height])
        .words(myWords.map(function(d){return {text: d.word, size: d.size};}))
        .padding(5) //space between words
        .rotate(function(){ return ~~(Math.random() * 2) * 90; })
        .fontSize(function(d) { return d.size*10; }) //font size of words
        .on("end", draw);
    layout.start();

    //This function takes the output of "layout" above and draw the words
    //wordCloud features that are the same from one word to the other can be here
    function draw(words){
        svg
          .append("g")
            .attr("transform", "translate(" + layout.size()[0] / 2 + "," + layout.size()[1] / 2 + ")")
            .selectAll("text")
                .data(words)
            .enter().append("text")
                .style("font-size", function(d) { return d.size + "px"; })
                .style("fill","black")
                .attr("text-anchor", "middle")
                .style("font-family", "Impact")
                .attr("transform", function(d){
                    return "translate(" + [d.x, d.y] + ")rotate(" + d.rotate + ")";
                })
                .text(function(d){ return d.text; });
    }

}

function callFunc(myForm){
    //Getting params
    const topic = myForm.topic.value;
    const from_date = myForm.from_date.value;
    const to_date = myForm.to_date.value;
    const src = myForm.news_sources.value;

    //Form url
    const url = "/search?topic=" + topic + "&from_date=" + from_date + "&to_date=" + to_date + "&news_sources=" + src;
	//GET = appends to URL, true = asynchronous
	const xmlhttp = new XMLHttpRequest();
	xmlhttp.open("GET", url, true);

	xmlhttp.onreadystatechange = function(){
	    if(xmlhttp.readyState == 4){
	        if(xmlhttp.status == 200){
	            const obj = JSON.parse(xmlhttp.responseText);
	            if(obj.status == "error"){
	                alert(obj.message);
	            }
	            else{
                    const queryArray = errorCheckingSingle(obj.articles);
                    writeQueryResult(queryArray);
	            }
	        }
	    }
	}
	xmlhttp.send();
}

function writeQueryResult(queryArray){
    const queryDiv = document.getElementById("queryDiv");
    queryDiv.innerHTML = " ";
    const max = 5;
    for(let i = 0; i < max; i++){
        const card = document.createElement("div");
        card.className = "queryResult";
        //card.style.width = "600px";
        //card.style.height = "127.5px";
        const resultContainerDiv = document.createElement("div");
        resultContainerDiv.className = "resultContainerDiv_class";
        /*resultContainerDiv.innerHTML = "<img src='" + queryArray[i].urlToImage + "'>";*/
        resultContainerDiv.innerHTML = "<img width='75px' height='75px' src='" + queryArray[i].urlToImage + "'><br><div class='initial_card'><b>" +
                         queryArray[i].title + "</b><br>" + queryArray[i].description + "</div>";
        resultContainerDiv.onclick = () => handleClick(card, queryArray[i]);

        card.appendChild(resultContainerDiv);
        queryDiv.appendChild(card);
    }

    const showMore = document.createElement("input");
    showMore.type = "button";
    showMore.value = "Show More";
    showMore.id = "showMore";

    queryDiv.appendChild(showMore);

    document.getElementById("showMore").addEventListener("click", () => showMoreFunc(queryArray));

}

function showMoreFunc(queryArray){
    document.getElementById("queryDiv").innerHTML = "";

    for(let i = 0; i < queryArray.length; i++){
        const card = document.createElement("div");
        card.className = "queryResult";
        const resultContainerDiv = document.createElement("div");
        resultContainerDiv.className = "resultContainerDiv_class";
        resultContainerDiv.innerHTML = "<img width='75px' height='75px' src='" + queryArray[i].urlToImage + "'><br><div class='initial_card'><b>" +
                         queryArray[i].title + "</b><br>" + queryArray[i].description + "</div>";
        resultContainerDiv.onclick = () => handleClick(card, queryArray[i]);
        card.appendChild(resultContainerDiv);
        document.getElementById("queryDiv").appendChild(card);
    }

    const showLess = document.createElement("input");
    showLess.type = "button";
    showLess.value = "Show Less";
    showLess.id = "showLess";

    document.getElementById("queryDiv").appendChild(showLess);
    document.getElementById("showLess").addEventListener("click", () => writeQueryResult(queryArray));

}

function handleClick(card, object){
    card.style.height = "227.5px";
    card.getElementsByClassName("resultContainerDiv_class")[0].style.height = "227.5px";
    const dateString = object.publishedAt;
    const yearSub = dateString.substring(0,4);
    const monthSub = dateString.substring(5,7);
    const daySub = dateString.substring(8,10);
    const final_date = monthSub + "/" + daySub + "/" + yearSub;
    card.getElementsByClassName("resultContainerDiv_class")[0].innerHTML = "";
    card.getElementsByClassName("resultContainerDiv_class")[0].innerHTML = "<img width='75px' height='75px' src='" + object.urlToImage + "'><br><div class='expanded_card'><b>" +
                     object.title + "</b><br><b>Author: </b>" + object.author +
                     "<br><b>Source: </b>" + object.source.name + "<br><b>Date: </b>" +
                     final_date + "<br>" + object.description +
                     "<br><a href='" + object.url + "' target='_blank'>See Original Post</a></div>";
    const close = document.createElement("div");
    close.className = "close_x";
    close.innerHTML = "X";
    close.onclick = () => makeItSmall(card, object);
    //card.onclick = " ";
    card.appendChild(close);
}

function makeItSmall(card, object){
        card.style.height = "127.5px";
        card.getElementsByClassName("resultContainerDiv_class")[0].style.height = "127.5px";
        //document.getElementsByClassName("close_x").onclick = " ";
        card.getElementsByClassName("resultContainerDiv_class")[0].innerHTML = "";
        card.getElementsByClassName("close_x")[0].remove();
        card.getElementsByClassName("resultContainerDiv_class")[0].innerHTML = "<img width='75px' height='75px' src='" + object.urlToImage + "'><br><div id= 'initial_card'><b>" +
                          object.title + "</b><br>" + object.description + "</div>";
        //card.onclick = () => handleClick(card, object);
}

/*
function handleClick(card, object){
    if(card.style.height == "127.5px"){
        card.style.height = "227.5px";
        const dateString = object.publishedAt;
        const yearSub = dateString.substring(0,4);
        const monthSub = dateString.substring(5,7);
        const daySub = dateString.substring(8,10);
        const final_date = monthSub + "/" + daySub + "/" + yearSub;
        card.innerHTML = " ";
        card.innerHTML = "<img src='" + object.urlToImage + "'><br><span><b>" +
                         object.title + "</b><br><b>Author: </b>" + object.author +
                         "<br><b>Source: </b>" + object.source.name + "<br><b>Date: </b>" +
                         final_date + "<br>" + object.description +
                         "<br><a href='" + object.url + "' target='_blank'>See Original Post</a></span>";
        const close = document.createElement("div");
        close.innerHTML = "X";
        close.onclick = () => handleClick(card, object);
        card.onclick = " ";
        card.appendChild(close);
    }
    else{
        card.style.height = "127.5px";
        close.onclick = " ";
        card.innerHTML = " ";
        card.innerHTML = "<img src='" + object.urlToImage + "'><br><span><b>" +
                          object.title + "</b><br>" + object.description + "</span>";
        card.onclick = () => handleClick(card, object);
    }


    /*const close = document.createElement("div");
    close.onclick = () => handleClose(card, object);
    close.innerHTML = "X";
    close.style = "background-color:red;";
    card.appendChild(close);*/
//}

/*function handleClose(card, object){
    card.style.height = "127.5px";
    //card.innerHTML = " ";
    console.log(card.innerHTML);
    card.innerHTML = "<img src='" + object.urlToImage + "'><br><span><b>" +
                        object.title + "</b><br>" + object.description + "</span>";
}*/
/*
function makeWordCloudList(dictionary){
    const dict_keys = Object.keys(dictionary);
    dict_keys.sort();

    var dict_sorted = [];

    for(let i = 0; i < dict_keys.length; i++){
        dict_sorted.push({'key': dict_keys[i], 'value': dictionary[dict_keys[i]]});
    }

    return dict_sorted;
}

function getFirstThirtyWord(previous_list){
    final_list = [];
    const max = 30;
    for(let i = 0; i < max; i++){
        final_list.push(previous_list[i].value);
    }

    return final_list;
}*/