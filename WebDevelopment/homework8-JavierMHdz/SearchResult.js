import React from "react";
import {Modal, Container, Row, Col, Spinner} from "react-bootstrap";
import {FacebookShareButton, FacebookIcon, TwitterShareButton, TwitterIcon, EmailShareButton, EmailIcon} from "react-share";
import {IoMdShare} from "react-icons/io";
import {Redirect} from 'react-router-dom';
import "./search_result_style.css";

class SearchResult extends React.Component{
    constructor(props){
        super(props);
        const loadingContent= (<Container className="spinnerStyle">
                                    <Col>
                                        <Row className="spinnerRow">
                                            <Spinner animation="grow" variant="primary" />
                                        </Row>
                                        <Row className="loadingRow">
                                            <p>Loading</p>
                                        </Row>
                                    </Col>
                                </Container>
                                );
        this.state ={
            content: loadingContent,
            show: false,
            article_title: "",
            article_url: "",
            id: "",
            section: "",
            detailed_show: false
        };
        this.getArticles = this.getArticles.bind(this);
        this.getArticle = this.getArticle.bind(this);
        this.getImage = this.getImage.bind(this);
        this.getSection = this.getSection.bind(this);
        this.getCard = this.getCard.bind(this);
        this.handleClick = this.handleClick.bind(this);
        this.handleShow = this.handleShow.bind(this);
        this.handleClose = this.handleClose.bind(this);
        this.detailedArticleShow = this.detailedArticleShow.bind(this);
        this.shareFunc = this.shareFunc.bind(this);
    }

    getImage(articles, i){
        let imgLink = "";

        //Guardian Image
        //If main is defined (avoids crashing)
        if(articles[i].blocks.main){
            const arraySize = articles[i].blocks.main.elements[0].assets.length;
            const lastIndex = arraySize - 1;
    
            //If the last index is defined
            if(articles[i].blocks.main.elements[0].assets[lastIndex]){
                imgLink = articles[i].blocks.main.elements[0].assets[lastIndex].file;
            }
            //else use default image
            else{
                imgLink = "https://assets.guim.co.uk/images/eada8aa27c12fe2d5afa3a89d3fbae0d/fallback-logo.png";
            }

        }
        //If main is undefined (avoids crashing)
        else{
            imgLink = "https://assets.guim.co.uk/images/eada8aa27c12fe2d5afa3a89d3fbae0d/fallback-logo.png";
        }

        return imgLink;
    }

    getSection(articles, i){
        let sectionClass = "";
        const section = articles[i].sectionId;
            
        if(section == "world"){
            sectionClass = "world-tab";
        }
        else if(section == "politics"){
            sectionClass = "politics-tab";
        }
        else if(section == "business"){
            sectionClass = "business-tab";
        }
        else if(section == "technology"){
            sectionClass = "technology-tab";
        }
        else if(section == "sport"){
            sectionClass = "sports-tab";
        }
        else{
            sectionClass = "any-other-tab";
        }

        return sectionClass;
    }

    handleClose = () => {
        this.setState(
            {
                ...this.state,
                show: false,
                article_title: "",
                article_url: ""
            }
        )
    }
    handleShow = (title, url) =>{
        this.setState(
            {
                ...this.state,
                show: true,
                article_title: title,
                article_url: url
            }
        )
    }

    detailedArticleShow = (articleID, articleSection) =>{
        this.setState(
            {
                ...this.state,
                detailed_show: true,
                id: articleID,
                section: articleSection
            }
        )
    }

    handleClick(target, title, url, articleID, articleSection){
        
        if(target.getAttribute("id") == "shareClick"){
            this.handleShow(title, url);
        }
        else{
            this.detailedArticleShow(articleID, articleSection);
            
        }
    }

    shareFunc(){
        const title = this.state.article_title;
        const socialMediaUrl = this.state.article_url;
        return (
        <>        
            <Modal backdrop="static" show={this.state.show} onHide={this.handleClose}>
            <Modal.Header closeButton>
                <Modal.Title>{title}</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                <Row style={{justifyContent: "center", marginBottom: "1vh", fontSize: "3vh"}}>Share via</Row>
                <Row style={{justifyContent: "center"}}>
                    <FacebookShareButton hashtag="#CSCI_571_NewsApp" url={socialMediaUrl}><FacebookIcon size={52} round={true} /></FacebookShareButton>
                    <span style={{paddingLeft: "15vh", paddingRight: "15vh"}}>
                        <TwitterShareButton hashtags={["CSCI_571_NewsApp"]} url={socialMediaUrl}><TwitterIcon size={52} round={true} /></TwitterShareButton>
                    </span>
                    <EmailShareButton subject={"#CSCI_571_NewsApp"} url={socialMediaUrl}><EmailIcon size={52} round={true} /></EmailShareButton>
                </Row>
            </Modal.Body>
            </Modal>
        </>
        );
    }

    getCard(articles, i, imgLink, sectionClass){
    let title = "";
    let date = "";

    //Guardian
    title = articles[i].webTitle;
    date = articles[i].webPublicationDate.substr(0,10);

    const articleSection = articles[i].sectionId.toUpperCase();

    return(
        <Col className="outsideContainer">
            <Container className="searchContentContainer" id="detailClick" onClick={(source) => this.handleClick(source.target, articles[i].webTitle, articles[i].webUrl, articles[i].id, articleSection)}>
                <Container className="innerMarginContainer">
                    <Row>
                        <p className="searchTitle">{title}</p>
                        <IoMdShare id="shareClick"></IoMdShare>
                    </Row>
                    <Row className="imageContainer">
                        <img className="search_result_image" src={imgLink} />
                    </Row>
                    <Row className="imageBottomRow">
                        <Col>{date}</Col>
                        <Col>
                            <span className={sectionClass}>{articles[i].sectionId}</span>
                        </Col>
                    </Row>
                </Container>
            </Container>
        </Col>
        )

    }

    getArticle(articles, i){
        //Get the image link
        let imgLink = this.getImage(articles, i);

        //Get the section
        let sectionClass = this.getSection(articles,i);

        return this.getCard(articles, i, imgLink, sectionClass);
    }

    getArticles(articles){
        const array = [];
        let maxNum = 0;
        if(articles.length > 10){
            maxNum = 10;
        }
        else{
            maxNum = articles.length
        }
        for(let i = 0; i < maxNum; i++){
            array.push(this.getArticle(articles, i));
        }
    
        return array;
    }

    componentDidMount(){
        this.data((obj) => this.updateState(obj));
    }

    updateState = (obj) => {
        let newContent = "";

        //obj.response.content.id
        newContent = this.getArticles(obj.response.results);

        this.setState({
            content: newContent
        });
    }

    data = (callbackFunc) =>{
        const xmlhttp = new XMLHttpRequest();
        //GET = appends to URL, true = asynchronous
        const word = this.props.location.pathname.split("/",3)[2];
        //console.log(word);
        const url = "https://backend-hw8.wl.r.appspot.com/search/guardian/keyword/" + word;
        xmlhttp.open("GET", url, true);

        xmlhttp.onreadystatechange = function(){
            if(xmlhttp.readyState == 4){
                if(xmlhttp.status == 200){
                    const obj = JSON.parse(xmlhttp.responseText);
                    callbackFunc(obj);
                }
            }
        }
        xmlhttp.send();
    }

    render(){
        return (
            <>
                <h1 style={{color:"grey"}}>Results</h1>
                <Row className="searchFinalDiv" sm={12} md={4} lg={4} xl={4}>
                    
                    {this.state.show && this.shareFunc()}
                    {this.state.detailed_show ? <Redirect to={'/detailed_article/guardian/' + this.state.section + '/' + this.state.id} source="guardian" id={this.state.id} section={this.state.section}/> : this.state.content}
                </Row>
            </>
        );
    }
}

export default SearchResult;