import React from "react";
import {Container, Row, Col} from "react-bootstrap";
import ReactDOMServer from 'react-dom/server';
import commentBox from "commentbox.io";
import {FacebookShareButton, FacebookIcon, TwitterShareButton, TwitterIcon, EmailShareButton, EmailIcon} from "react-share";
import {FaBookmark, FaRegBookmark, FaAngleDown, FaAngleUp} from "react-icons/fa";
import {ToastContainer, toast} from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css'
import "./detailed_article_style.css";

class DetailedArticleShow extends React.Component{

    constructor(props){
        super(props);
        this.state = {
            articlesArray: [],
            title: "",
            date: "",
            socialMediaUrl: "",
            section: "",
            imgLink: "",
            description: "",
            save: false,
            show: false
        };
        this.createDetailedArticle = this.createDetailedArticle.bind(this);
        this.storeClick = this.storeClick.bind(this);
        this.showDescription = this.showDescription.bind(this);
        this.checkIfSaved = this.checkIfSaved.bind(this);
        this.deleteClick = this.deleteClick.bind(this);
    }

    checkIfSaved(){
        const switch_source = this.props.source;
        const article = this.props.article;
        let articleID = "";
        if(switch_source == "guardian"){
            articleID = article.id;
        }
        else{
            articleID = article.web_url;
        }
        const keys = Object.keys(localStorage);
        const maxNum = keys.length;
       //let flag = false;
        for(let i = 0; i < maxNum; i++){
            if(articleID == keys[i]){
                //flag = true;
                // this.setState({
                //     save: true
                // })
                return true;
            }
        }

        return false;

        //return flag;
    }

    deleteClick(){
        const source = this.props.source;
        const article = this.props.article;
        if(source == "guardian"){
            localStorage.removeItem(article.id);
            toast("Removing " + article.webTitle, {position: toast.POSITION.TOP_CENTER});
        }
        else{
            localStorage.removeItem(article.web_url);
            toast("Removing " + article.headline.main, {position: toast.POSITION.TOP_CENTER});
        }
        this.setState({
            ...this.state,
            save: false
        });
    }

    // notifySaved = () =>{
    //     toast("Saving " + this.state.title, {containerId: 'savedMessage'});
    // }


    storeClick(){
        const source = this.props.source;
        const article = this.props.article;
        let storedObj ={};

        if(source === "guardian"){
            storedObj = {
                id: article.id,
                title: this.state.title,
                date: this.state.date,
                socialMediaUrl: this.state.socialMediaUrl,
                section: this.props.section,
                imgLink: this.state.imgLink,
                description: this.state.description,
                source: this.props.source
            }
            //Checking browser support for local storage and sessionstorage
            if(typeof(Storage) !== "undefined"){
                //Store information
                localStorage.setItem(article.id,JSON.stringify(storedObj));
                //this.notifySaved();
                toast("Saving " + this.state.title, {position: toast.POSITION.TOP_CENTER});
                //alert("Saving " + this.state.title);
            }

            this.setState({
                ...this.state,
                save: true
            });
        }
        else{
            storedObj = {
                id: article.web_url,
                title: this.state.title,
                date: this.state.date,
                socialMediaUrl: this.state.socialMediaUrl,
                section: this.props.section,
                imgLink: this.state.imgLink,
                description: this.state.description,
                source: this.props.source
            }
            //console.log(article);
            //Checking browser support for local storage and sessionstorage
            if(typeof(Storage) !== "undefined"){
                //Store information
                localStorage.setItem(article.web_url,JSON.stringify(storedObj));
                //this.notifySaved();
                toast("Saving " + this.state.title, {position: toast.POSITION.TOP_CENTER});
                //alert("Saving " + this.state.title);
            }

            this.setState({
                ...this.state,
                save: true
            });
        }
        

    }

    createDetailedArticle(){
        const source = this.props.source;
        const article = this.props.article;
        let title = "";
        let imgLink = "";
        let date = "";
        let description = "";
        let socialMediaUrl = "";

        if(source === "guardian"){
            title = article.webTitle;
            date = article.webPublicationDate.substr(0,10);
            description = article.blocks.body[0].bodyTextSummary;
            socialMediaUrl = article.webUrl;

            //If main is defined (avoids crashing)
            if(article.blocks.main){
                const arraySize = article.blocks.main.elements[0].assets.length;
                const lastIndex = arraySize - 1;

                //If the last index is defined
                if(article.blocks.main.elements[0].assets[lastIndex]){
                    imgLink = article.blocks.main.elements[0].assets[lastIndex].file;
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
            
        }
        //NYTimes
        else{
            title = article.headline.main;
            date = article.pub_date.substr(0,10);
            description = article.abstract;
            socialMediaUrl = article.web_url;
            //console.log("article here: " + article);

            if(article.multimedia){
                const arraySize = article.multimedia.length;
                for(let k = 0; k < arraySize; k++){
                    if(article.multimedia[k].width >= 2000){
                        imgLink = "https://static01.nyt.com/" + article.multimedia[k].url;
                        break;
                    }
                }
                
            }
            //Condition: articles[i].multimedia is undefined or width is less than 2000
            if(imgLink == ""){
                imgLink = "https://upload.wikimedia.org/wikipedia/commons/0/0e/Nytimes_hq.jpg";
            }
            
        }

        

        this.setState({
            ...this.state,
            title: title,
            date: date,
            socialMediaUrl: socialMediaUrl,
            description: description,
            imgLink: imgLink,
            //exist: checker
        })
    }

    componentDidMount(){
        this.checkIfSaved();
        // //this.checkIfSaved();
        // console.log("checker value: " + checker);
        // this.setState({
        //      ...this.state,
        //      save: checker
        //  });
        // console.log("this.state.save value: " + this.state.save);
        this.createDetailedArticle();
        this.removeCommentBox = commentBox("5689668049829888-proj");

    }

    componentWillUnmount(){
        this.removeCommentBox();
    }

    showDescription(){
        //console.log(document.getElementById("description_paragraph").classList);
        document.getElementById("description_paragraph").classList.toggle("paragraphBig");
        document.getElementById("description_paragraph").classList.toggle("paragraphSmall");
        //console.log(document.getElementById("description_paragraph").classList);
        if(document.getElementById("description_paragraph").classList.value === "paragraphBig"){
            document.getElementById("arrow_direction").innerHTML= ReactDOMServer.renderToStaticMarkup(<FaAngleUp />);
        }
        else{
            document.getElementById("arrow_direction").innerHTML= ReactDOMServer.renderToStaticMarkup(<FaAngleDown />);
        }
        
    }

    render(){
        return(
            <Container fluid className="detailedArticleOutsideContainer">
                <Container fluid className="detailedArticleInsideContainer">
                    <Row><h1 className="italicWords">{this.state.title}</h1></Row>
                    <Row className="iconRow">
                        <Col><p className="italicWords">{this.state.date}</p></Col>
                        <Col className="iconColumn">
                            <FacebookShareButton hashtag="#CSCI_571_NewsApp" url={this.state.socialMediaUrl}><FacebookIcon size={24} round={true} /></FacebookShareButton>     
                            <TwitterShareButton hashtags={["CSCI_571_NewsApp"]} url={this.state.socialMediaUrl} ><TwitterIcon size={24} round={true} /></TwitterShareButton>
                            <EmailShareButton subject={"#CSCI_571_NewsApp"} url={this.state.socialMediaUrl}><EmailIcon size={24} round={true} /></EmailShareButton>
                            {!this.checkIfSaved() ?
                                <FaRegBookmark style={{color: "red", marginLeft: "7vh"}} onClick={this.storeClick}/>
                                :
                                <FaBookmark style={{color: "red", marginLeft: "7vh"}} onClick={this.deleteClick}/>
                            }
                            {}
                        </Col>
                    </Row>
                    <Row>
                        <img className="detailed_article_image" src={this.state.imgLink} />
                    </Row>
                    <Row>
                        {(this.props.source == "guardian") ? <p className="paragraphSmall" id="description_paragraph">{this.state.description}</p> : <p className="paragraphBig">{this.state.description}</p>}
                        {(this.props.source == "guardian") ? <span className="arrow" id="arrow_direction" onClick={this.showDescription}><FaAngleDown/></span> : <span className="arrow"><FaAngleDown/></span>}
                        
                    
                        {<ToastContainer hideProgressBar={true} bodyClassName="toast_style" autoClose={2500}/>}
                    </Row>
                </Container>
                <div className="commentbox">
                </div>
            </Container>
        );
    }

}

export default DetailedArticleShow;