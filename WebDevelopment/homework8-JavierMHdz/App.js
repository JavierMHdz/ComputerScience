import NYTimesGet from './components/NYTimesGet';
import GuardianGet from './components/GuardianGet';
import DetailedArticle from './components/DetailedArticle.js';
import DetailedArticleShow from './components/DetailedArticleShow.js';
import Section from './components/Section.js';
import NavBar from './components/NavBar';
import SearchResult from './components/SearchResult.js';
import FavoriteArticles from './components/FavoriteArticles.js'
import React from 'react';
import './App.css';
import {Switch, Route, Redirect} from 'react-router-dom';
import Container from 'react-bootstrap/Container';


class App extends React.Component{

  //Check if it is Guardian or NYTimes
  constructor(props){
    super(props);
    this.state = {
      source: true,
      // colorChanged: true
    }
    this.handleSource = this.handleSource.bind(this);
  }

  handleSource(){

    this.setState(
      {
        ...this.state,
        source: !this.state.source,
      }
    )
  }
 
  render(){
    return (
        <div style={{width: '100%', height: '100%', padding: '0%', margin: '0%'}}>
          <NavBar checked={this.state.source} handleSource={this.handleSource} />
          <Container fluid className="mainContainer">
            <Switch>
              
              <Route exact path = "/home/nytimes" component = {NYTimesGet}/>

              
              <Route exact path = "/home/guardian" component = {GuardianGet}/>

              
              <Route exact path = "/world/nytimes" component = {() => <Section source="nytimes" section="world"/>} />
              <Route exact path = "/politics/nytimes" component = {() => <Section source="nytimes" section="politics"/>} />
              <Route exact path = "/business/nytimes" component = {() => <Section source="nytimes" section="business"/>} />
              <Route exact path = "/technology/nytimes" component = {() => <Section source="nytimes" section="technology"/>} />
              <Route exact path = "/sports/nytimes" component ={() => <Section source="nytimes" section="sports"/>} />

              
              <Route exact path = "/world/guardian" component = {() => <Section source="guardian" section="world"/>} />
              <Route exact path = "/politics/guardian" component = {() => <Section source="guardian" section="politics"/>} />
              <Route exact path = "/business/guardian" component = {() => <Section source="guardian" section="business"/>} />
              <Route exact path = "/technology/guardian" component = {() => <Section source="guardian" section="technology"/>} />
              <Route exact path = "/sport/guardian" component ={() => <Section source="guardian" section="sport"/>} />

              
              <Route path = "/detailed_article" component={DetailedArticle} />

              
              <Route path = "/search_result" component={SearchResult} />
              

            
              <Route path = "/favorite_articles" component={FavoriteArticles} />



            </Switch>
          </Container>
          {
            this.state.source ? <Redirect to = '/home/guardian' /> : <Redirect to = '/home/nytimes' />
          }
          
        </div>
    );
  }
}

export default App;
