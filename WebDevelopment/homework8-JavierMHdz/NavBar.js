import React from "react";
import {FaBookmark, FaRegBookmark} from "react-icons/fa";
import Switch from "react-switch";
import {Navbar, Nav, Form} from "react-bootstrap";
import {NavLink, Redirect} from 'react-router-dom';
import _ from "lodash";
import data from "./searchBoxAPI.js";
import "./navbar_style.css";
import Select from 'react-select';

class NavBar extends React.Component{
    constructor(props){
        super(props);
        this.state = {
            favorite: false,
            suggestion_array: [],
            querying: false,
            word: ""
        };
        this.favoriteClick = this.favoriteClick.bind(this);
        this.callbackFunc = this.callbackFunc.bind(this);
        this.handleChange = this.handleChange.bind(this);
        this.initState = this.initState.bind(this);
    }

    initState(){
        this.setState({
            ...this.state,
            favorite: false
        })

    }

    handleChange(query){

        this.setState({
            ...this.state,
            querying: !this.state.querying,
            word: query.value
        });

    }

    //Call back function
    callbackFunc(obj){
        //console.log("here");
        const init_array = obj.suggestionGroups[0].searchSuggestions;
        const final_array = [];
        for(let i = 0; i < init_array.length; i++){
            final_array.push({label: init_array[i].displayText, value: init_array[i].displayText});
        }

        //To re render
        this.setState({
            suggestion_array: final_array,
            querying: !this.state.quering,
            word: ""
        });
    }

    favoriteClick(){
        this.setState({
            ...this.state,
            favorite: !this.state.favorite
        })
    }

    render(){
        let source = "";
        if(this.props.checked){
            source = "guardian";
        }
        else{
            source = "nytimes";
        }
        return(
                <Navbar expand="lg" variant="dark" style={{backgroundColor: "#1f3d7a", width:"100%"}}>
                    <Form inline>
                        <Select
                            options={this.state.suggestion_array}
                            placeholder="Enter keyword..."
                            className="search_box"
                            noOptionsMessage={() => "No Match"}
                            loadingMessage={() => "Loading..."}
                            onInputChange={_.debounce(
                                query => data(query,this.callbackFunc), 1000
                            )}
                            onChange={(query) => this.handleChange(query)}
                            value={this.state.word}
                        />
                    </Form>
                    <Nav className="mr-auto">
                        <Nav.Link as={NavLink} to={'/home/' + source} onClick={this.initState}>Home</Nav.Link>
                        <Nav.Link as={NavLink} to={'/world/' + source} onClick={this.initState}>World</Nav.Link>
                        <Nav.Link as={NavLink} to={'/politics/' + source} onClick={this.initState}>Politics</Nav.Link>
                        <Nav.Link as={NavLink} to={'/business/' + source} onClick={this.initState}>Business</Nav.Link>
                        <Nav.Link as={NavLink} to={'/technology/' + source} onClick={this.initState}>Technology</Nav.Link>
                        <Nav.Link as={NavLink} to={(source == "guardian") ? '/sport/guardian' : '/sports/nytimes'} onClick={()=>this.initState()}>Sports</Nav.Link>
                    </Nav>
                    <Nav.Link style={{padding: "0"}} as={NavLink} to={!this.state.favorite ? '/favorite_articles' : (this.props.checked ? '/home/guardian' : '/home/nytimes')}>
                        { !this.state.favorite ?
                            <FaRegBookmark style={{color: "white", padding:"0"}} onClick={this.favoriteClick}/>
                            :
                            <FaBookmark style={{color: "white", padding:"0"}} onClick={this.favoriteClick}/>
                        }
                    </Nav.Link>
                    { !this.state.favorite &&
                        (<><span style={{marginLeft: "1vh", marginRight:"1vh", color: "white"}}>NY Times </span>
                        <Switch onChange={this.props.handleSource} checked={this.props.checked} onColor='#0099ff' offColor='#d9d9d9' uncheckedIcon={false} checkedIcon={false}/>
                        <span style={{marginLeft: "1vh", color: "white"}}> Guardian</span></>)
                     }
                     {(this.state.word !== "") && <Redirect to={"/search_result/" + this.state.word}/>}
                </Navbar>
        );
       
    }
}

export default NavBar;