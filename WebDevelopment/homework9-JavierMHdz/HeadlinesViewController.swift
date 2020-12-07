//
//  HeadlinesViewController.swift
//  homework9
//
//  Created by Javier Jesús Macossay Hernández on 5/4/20.
//  Copyright © 2020 Javier Jesús Macossay Hernández. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import SwiftSpinner

class HeadlinesViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    @IBOutlet weak var headlinesTable: UITableView!
    
    var timer: Timer!
    
    var json:[JSON] = [];
    
    let url = "https://backend-hw9.wl.r.appspot.com/search/guardian/business";
    //let url = "http://localhost:5000/search/guardian/business"

    override func viewDidLoad() {
        super.viewDidLoad()
        SwiftSpinner.show("Loading Headlines Page...");
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

            self.headlinesTable.reloadData();
        }
    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return json.count
    }

    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "headlinesCell", for: indexPath) as? HeadlinesTableCell
        cell?.titleLabel.text = json[indexPath.row]["webTitle"].rawString()!
        cell?.sectionLabel.text = json[indexPath.row]["sectionName"].rawString()!
        
        return cell!
    }
    
}
