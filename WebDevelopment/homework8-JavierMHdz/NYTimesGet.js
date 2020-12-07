import React from "react";
import {Modal, Container, Row, Col, Spinner} from "react-bootstrap";
import {FacebookShareButton, FacebookIcon, TwitterShareButton, TwitterIcon, EmailShareButton, EmailIcon} from "react-share";
import {IoMdShare} from "react-icons/io";
import {Redirect} from 'react-router-dom';
import "./homecards.css";

class NYTimesGet extends React.Component{
    constructor(props){
        super(props);
        this.state = {
            content: <Container className="spinnerStyle"><Col><Row className="spinnerRow"><Spinner animation="grow" variant="primary" /></Row><Row className="loadingRow"><p>Loading</p></Row></Col></Container>,
            show: false,
            article_title: "",
            article_url: "",
            id: "",
            section: "",
            detailed_show: false
        };
        this.shareFunc = this.shareFunc.bind(this);
        this.getArticle = this.getArticle.bind(this);
        this.getArticles = this.getArticles.bind(this);
        this.handleClick = this.handleClick.bind(this);
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

    getArticle(articles, i){
        let imgLink = "";
        if(articles[i].multimedia){
            const arraySize = articles[i].multimedia.length;
            for(let k = 0; k < arraySize; k++){
                if(articles[i].multimedia[k].width >= 2000){
                    imgLink = articles[i].multimedia[k].url;
                    break;
                }
            }
            
        }

        //Condition: articles[i].multimedia is undefined or width is less than 2000
        if(imgLink == ""){
            imgLink = "https://upload.wikimedia.org/wikipedia/commons/0/0e/Nytimes_hq.jpg";
        }

        const section = articles[i].section;
        let sectionClass = "";
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
        else if(section == "sports"){
            sectionClass = "sports-tab";
        }
        else{
            sectionClass = "any-other-tab";
        }

       

        const articleSection = articles[i].section.toUpperCase();
    
        return(
            <Container className="initCard" fluid key= {'ny_'+ i } id="detailClick" onClick={(source) => this.handleClick(source.target, articles[i].title, articles[i].url, articleSection)}>
                <Row>
                    <Col xs={12} sm={12} md={2} lg={2} xl={2}><img src={imgLink} className="initCardImg" alt = ''/></Col>
                    <Col className="initCardContent">
                        <p><b>{articles[i].title} </b><IoMdShare id="shareClick"></IoMdShare></p>                        
                        <p className="initCardText">{articles[i].abstract}</p>
                        <p>{articles[i].published_date.substr(0,10)}
                            <span className={sectionClass}>{articleSection}</span>
                        </p> 
                    </Col>
                </Row>
            </Container>
        )
    }

    handleClick(target, title, url, articleSection){
        if(target.getAttribute("id") == "shareClick"){
            this.handleShow(title, url);
        }
        else{
            this.detailedArticleShow(url, articleSection);
        }
    }
    
    getArticles(articles){
        const array = [];
        for(let i = 0; i < articles.length; i++){
            array.push(this.getArticle(articles,i));
        }
    
        return array;
    }

    componentDidMount(){
        this.data((obj) => this.updateState(obj));
    }

    updateState = (obj) => {
        const newContent = this.getArticles(obj.results);
        this.setState({
            content: newContent
        });
    }

    data = (callbackFunc) =>{
        const xmlhttp = new XMLHttpRequest();

        //GET = appends to URL, true = asynchronous
        const url = "https://backend-hw8.wl.r.appspot.com/search/nytimes";
        xmlhttp.open("GET", url, true);

        xmlhttp.onreadystatechange = function(){
            if(xmlhttp.readyState === 4){
                if(xmlhttp.status === 200){
                    const obj = JSON.parse(xmlhttp.responseText);
                    callbackFunc(obj);
                }
            }
        }
        xmlhttp.send();
    }

    render (){
        return(
            <>
                {this.state.show && this.shareFunc()}
                {this.state.detailed_show ? <Redirect to={'/detailed_article/nytimes/' + this.state.section + "/" + this.state.id} source="nytimes" id={this.state.id} section={this.state.section}/> : this.state.content}
            </>
        )
    }
}

export default NYTimesGet;