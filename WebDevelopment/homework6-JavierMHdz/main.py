from newsapi import NewsApiClient
from flask import Flask, request, current_app

# Initialize
newsapi = NewsApiClient(api_key='c0a3d0618eff474a9b6e17f56cf4f18c')

# /v2/top-headlines
#top_headlines = newsapi.get_top_headlines(q='coronavirus',
                                          #sources='bbc-news,the-verge')

top_headlines = newsapi.get_top_headlines(sources='cnn, fox-news', page_size=30)

all_top_headlines = newsapi.get_top_headlines(sources='', page_size=30)

# /v2/everything

#sources = newsapi.get_sources()

def readTextFile():

    commonWords = []
    with current_app.open_resource('static/stopwords_en.txt') as file1:
        for line in file1:
            readyLine = line.strip().decode('utf-8')
            commonWords.append(readyLine)
    file1.close()

    return commonWords



def computeWordCloud(commonWords):
    dictionary = {}
    for individual_dict in all_top_headlines["articles"]:
        string = individual_dict["title"]
        word_list = string.split()
        for word in word_list:
            if word.lower() in commonWords or word.isnumeric() or not word.isalnum():
                continue

            if word.lower() in dictionary:
                dictionary[word.lower()] += 1
            else:
                dictionary[word.lower()] = 1

    return dictionary

def dict_to_list(dictionary):
    list = []
    final_list = []
    #Sort the dictionary into the list
    list = sorted(dictionary.items(), key=lambda x: x[1], reverse=True)

    #Now compute the final list
    for word in list:
        object = {"word": word[0], "size": word[1]}
        final_list.append(object)

    return final_list

app = Flask(__name__)

@app.route('/')
# From slides
def homepage():
    return app.send_static_file("index.html")

@app.route('/sources')
def getSources():
    return newsapi.get_sources()

@app.route('/headlines')
def headlinesFunc():
    #Carousel
    top_headlines["carousel_articles"] = all_top_headlines["articles"]

    # #WordCloud
    commonWords = readTextFile()
    word_cloud_dict = computeWordCloud(commonWords)
    word_cloud_list = dict_to_list(word_cloud_dict)
    top_headlines["word_cloud"] = word_cloud_list[0:30]

    return top_headlines

@app.route('/search')
def searchFunc():
    try:
        all_articles = newsapi.get_everything(q=request.args["topic"],
                                              sources=request.args["news_sources"],
                                              from_param=request.args["from_date"],
                                              to=request.args["to_date"],
                                              language='en',
                                              page_size=30)

        return all_articles
    except Exception as error:
        return error.args[0]



if __name__ == "__main__":
    app.run()

