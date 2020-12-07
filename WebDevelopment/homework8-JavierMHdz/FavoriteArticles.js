import React from "react";
import {Modal, Container, Row, Col, Spinner} from "react-bootstrap";
import {FacebookShareButton, FacebookIcon, TwitterShareButton, TwitterIcon, EmailShareButton, EmailIcon} from "react-share";
import {IoMdShare} from "react-icons/io";
import {Redirect} from 'react-router-dom';
import {FaTrash} from "react-icons/fa";
import {ToastContainer, toast} from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css'
import "./search_result_style.css";

class FavoriteArticles extends React.Component{
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
            articlesArray: [],
            show: false,
            article_title: "",
            article_url: "",
            id: "",
            section: "",
            api_source: "",
            detailed_show: false
        };
        this.getFromMemory = this.getFromMemory.bind(this);
        this.shareFunc = this.shareFunc.bind(this);
        this.getArticles = this.getArticles.bind(this);
        this.getCard = this.getCard.bind(this);
        this.handleClick = this.handleClick.bind(this);
        this.deleteClick = this.deleteClick.bind(this);
    }

    deleteClick(id, title){
        localStorage.removeItem(id);
        toast("Removing " + title, {position: toast.POSITION.TOP_CENTER});
        const articles = this.getFromMemory();
        let newContent = "";
        newContent = this.getArticles(articles);

        this.setState({
            content: newContent,
            articlesArray: articles
        });
    }

    handleClose = () => {
        this.setState({
            ...this.state,
            show: false,
            article_title: "",
            article_url: ""
        });
    }

    handleShow = (title, url) =>{
        this.setState({
            ...this.state,
            show: true,
            article_title: title,
            article_url: url
        });
    }

    detailedArticleShow = (articleID, articleSection, switch_source) =>{
        this.setState(
            {
                ...this.state,
                detailed_show: true,
                id: articleID,
                section: articleSection,
                api_source: switch_source
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

    getFromMemory(){
        const values = [];
        const keys = Object.keys(localStorage);
        const maxNum = keys.length;

        for(let i = 0; i < maxNum; i++){
            values.push(JSON.parse(localStorage.getItem(keys[i])));
        }
        return values;
    }

    getArticles(articles){
        const array = [];
        const maxNum = articles.length;
        for(let i = 0; i < maxNum; i++){
            array.push(this.getCard(articles, i));
        }
    
        return array;
    }

    getCard(articles, i){
        const title = articles[i].title;
        const date = articles[i].date;
        const section = articles[i].section;
        const imgLink = articles[i].imgLink
        const url = articles[i].socialMediaUrl;
        const id = articles[i].id;
        const switch_source = articles[i].source;
        let sourceClass = "";
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
        else if(section == "sport"){
            sectionClass = "sports-tab";
        }
        else{
            sectionClass = "any-other-tab";
        }

        if(switch_source == "guardian"){
            sourceClass = "guardian-tab";
        }
        else{
            sourceClass = "nytimes-tab";
        }

        return(
            <Col className="outsideContainer" id="1">
                <Container className="searchContentContainer" id="2" onClick={(e) => this.handleClick(e, title, url, id, section, switch_source)}>
                    <Container className="innerMarginContainer" id="3">
                        <Row id="4">
                            <p className="searchTitle" id="7">{title}</p>
                            
                        </Row>
                        
                        <IoMdShare id="shareClick"></IoMdShare>
                        <FaTrash id="trashClick" onClick={()=>this.deleteClick(id, title)}></FaTrash>
                        
                        <Row className="imageContainer" id="5">
                            <img className="search_result_image" src={imgLink} id="8"/>
                        </Row>
                        <Row className="imageBottomRow" id="6">
                            <Col id="9">{date}</Col>
                            <Col>
                                <span className={sectionClass} style={{fontStyle:"normal", fontSize: "2vh"}} id="10">{section.toUpperCase()}</span>
                                <span className={sourceClass} style={{fontStyle:"normal", fontSize: "2vh"}} id="11">{switch_source.toUpperCase()}</span>
                            </Col>
                        </Row>
                    </Container>
                </Container>
            </Col>
        )
    
    }

    handleClick(e, title, url, articleID, articleSection, switch_source){
        
        const my_id = e.target.getAttribute("id");
        if(my_id == "shareClick"){
            this.handleShow(title,url);
        }
        else if((my_id == "1") || (my_id == "2") || (my_id == "3") || (my_id == "4") || (my_id == "5") || (my_id == "6") || (my_id == "7") || (my_id == "8") || (my_id == "9") || (my_id == "10") || (my_id == "11") || (my_id == "12")){
            //this.deleteClick(articleID, title);
            this.detailedArticleShow(articleID, articleSection, switch_source);
        }
        
    }



    componentDidMount(){
        const articles = this.getFromMemory();

        let newContent = "";
        newContent = this.getArticles(articles);

        this.setState({
            content: newContent,
            articlesArray: articles
        });
    }

    render(){        
        return (
            (this.state.articlesArray.length > 0) ?
            <>
            <h1 style={{color:"grey"}}>Favorites</h1>
                <Row className="searchFinalDiv" sm={12} md={4} lg={4} xl={4}>
                    {this.state.show && this.shareFunc()}
                    {this.state.detailed_show ? <Redirect to={'/detailed_article/' + this.state.api_source + '/' + this.state.section +'/'+ this.state.id}/> : this.state.content} 
                </Row>
                <ToastContainer hideProgressBar={true} bodyClassName="toast_style" autoClose={2500}/>
            </>
            :
            <h1 style={{color:"black", textAlign:"center"}}>You have no saved articles</h1>
        );
    }

}

export default FavoriteArticles;