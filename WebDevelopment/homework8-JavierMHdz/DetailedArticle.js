import React from "react";
import DetailedArticleShow from "./DetailedArticleShow.js";
import {Container, Row, Col, Spinner} from "react-bootstrap";
import {FacebookShareButton, FacebookIcon, TwitterShareButton, TwitterIcon, EmailShareButton, EmailIcon} from "react-share";
import {FaRegBookmark, FaAngleDown, FaAngleUp} from "react-icons/fa";
import "./detailed_article_style.css";

class DetailedArticle extends React.Component{
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
        this.state = {
            content: loadingContent,
            show: false
        };
        //this.createDetailedArticle = this.createDetailedArticle.bind(this);
        //this.handleClick = this.handleClick.bind(this);
        this.data = this.data.bind(this);
        //this.showDescription = this.showDescription.bind(this);
        // this.showMore = this.showMore.bind(this);
        // this.hideMore = this.hideMore.bind(this);
    }

    componentDidMount(){
        this.data((obj) => this.updateState(obj));
    }

    updateState = (obj) =>{
        const switch_source = this.props.location.pathname.split("/",3)[2];
        let newContent = "";
        //console.log(this.props);

        const first_slash = this.props.location.pathname.indexOf("/");
        const second_slash = this.props.location.pathname.indexOf("/",(first_slash+1));
        const third_slash = this.props.location.pathname.indexOf("/",(second_slash+1));
        const fourth_slash = this.props.location.pathname.indexOf("/",(third_slash+1));
        const current_section = this.props.location.pathname.substring(third_slash+1, fourth_slash);
        //const current_id = this.props.location.pathname.substring(fourth_slash+1);

        /***look at backend to get the obj.response....->? */
        if(switch_source == "guardian"){
            //obj.response.content.id
            //newContent = this.createDetailedArticle(obj.response.content, "guardian");
            newContent = <DetailedArticleShow source="guardian" article={obj.response.content} section={current_section}/>;
        }
        else{
            //obj.response.docs.web_url
            //newContent = this.createDetailedArticle(obj.response.docs[0], "nytimes");
            //console.log(obj.response.docs[0]);
            newContent = <DetailedArticleShow source="nytimes" article={obj.response.docs[0]} section={current_section}/>;
        }

        this.setState({
            ...this.state,
            content: newContent,
        });
    }

    data(callbackFunc){
        const xmlhttp = new XMLHttpRequest();
        
        let url = "https://backend-hw8.wl.r.appspot.com/search/";
        const source = this.props.location.pathname.split("/",3)[2];
        const first_slash = this.props.location.pathname.indexOf("/");
        const second_slash = this.props.location.pathname.indexOf("/",(first_slash+1));
        const third_slash = this.props.location.pathname.indexOf("/",(second_slash+1));
        const fourth_slash = this.props.location.pathname.indexOf("/",(third_slash+1));
        //const section = this.props.location.pathname.substring(third_slash+1, fourth_slash);
        const id = this.props.location.pathname.substring(fourth_slash+1);

    

        url += source + "/detailed/" + encodeURIComponent(id);

        xmlhttp.open("GET", url, true);

        xmlhttp.onreadystatechange = function(){
            if(xmlhttp.readyState == 4){
                if(xmlhttp.status == 200){
                    
                    callbackFunc(JSON.parse(xmlhttp.responseText));
                }
            }
        }
        xmlhttp.send();

    }

    render(){
        return(
            <>
                {this.state.content}
            </>
        );
    }
}

export default DetailedArticle;