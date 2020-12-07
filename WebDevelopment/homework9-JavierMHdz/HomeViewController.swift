//
//  FirstViewController.swift
//  homework9
//
//  Created by Javier Jesús Macossay Hernández on 5/1/20.
//  Copyright © 2020 Javier Jesús Macossay Hernández. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import SwiftSpinner
//import Alamofire_SwiftyJSON

class HomeViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    @IBOutlet weak var homeTable: UITableView!
    
    //var articleID:String = "";
    
//    let json = [["title":"first title", "description":"first description"],
//                ["title":"second title", "description": "second description"]]
    
    var timer: Timer!
    
    //var json:DataResponse<JSON>;
    var json:[JSON] = [];
    //let url = URL(string: "https://localhost:5000/search/guardian");
    let url = "https://backend-hw9.wl.r.appspot.com/search/guardian";
    //let url = "http://localhost:5000/search/guardian";
    
//    var myIndex = 0;

    
    override func viewDidLoad() {
        super.viewDidLoad()
        SwiftSpinner.show("Loading Home Page...");
        // Do any additional setup after loading the view, typically from a nib.
        Alamofire.request(url, method: .get).validate().responseJSON{ response in
            switch response.result{
            case .success(let value):
                let results = JSON(value)["response"]["results"]
                let size = results.count-1
                for n in 1...size{
                    self.json.append(results[n])
                }
                SwiftSpinner.hide();
                //print("JSON: \(self.json)")
            case .failure(let error):
                print(error)
            }
            
            self.homeTable.reloadData();
        }
        
//        Alamofire.request(url).responseJSON{
//            (responseData) -> Void in
//            if((responseData.result.value) != nil){
//                let swiftyJsonVar = JSON(responseData.result.value!);
//            }
//        }
        
        
        

//        timer = Timer.scheduledTimer(timeInterval: 1.0, target: self, selector: #selector(spinnerOn), userInfo: nil, repeats: false);
//        timer = Timer.scheduledTimer(timeInterval: 2.0, target: self, selector: #selector(spinnerOff), userInfo: nil, repeats: false);
        
//        timer = Timer.scheduledTimer(timeInterval: 2.0, target: self, selector: #selector(spinnerOff), userInfo: nil, repeats: false);
    }
    
//    func spinnerOn(){
       //SwiftSpinner.show("Loading Home Page...");
    //}
    
//    @objc func spinnerOff(){
//        SwiftSpinner.hide();
//    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int{
        return json.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell{
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath) as? NewsTableCell
        cell?.titleLabel.text = json[indexPath.row]["webTitle"].rawString()!
        cell?.sectionLabel.text = json[indexPath.row]["sectionName"].rawString()!
//        cell?.dateLabel.text = json[indexPath.row]["webPublicationDate"].rawString()!
//        let imageUrl = json[indexPath.row]["fields"]["thumbnail"].rawString()!
//        let myImage = UIImage(named: imageUrl)
//        cell?.imageSpace = UIImageView(image: myImage)

//        cell?.titleLabel.text = json[indexPath.row]["title"]
//        cell?.descriptionLabel.text = json[indexPath.row]["description"]
        return cell!
    }
    
//    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
//        //Going to be the cell the user tapped on
//        //myIndex = indexPath.row
//        performSegue(withIdentifier: "segue", sender: articleID)
//    }

}

