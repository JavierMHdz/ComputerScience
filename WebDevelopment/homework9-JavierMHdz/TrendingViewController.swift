//
//  TrendingViewController.swift
//  homework9
//
//  Created by Javier Jesús Macossay Hernández on 5/4/20.
//  Copyright © 2020 Javier Jesús Macossay Hernández. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON
import SwiftSpinner
import Charts
import TinyConstraints

class TrendingViewController: UIViewController, ChartViewDelegate{
    
    lazy var lineChartView: LineChartView = {
        let chartView = LineChartView()
        //chartView.backgroundColor = UIColor.blue
        return chartView
    }()
    
    let url = "https://backend-hw9.wl.r.appspot.com/trending/coronavirus";
    //let url = "http://localhost:5000/trending/coronavirus"
    
    
    var json:[JSON] = [];
    var yValues: [ChartDataEntry] = [];
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        Alamofire.request(url, method: .get).validate().responseJSON{ response in
            switch response.result{
            case .success(let value):
                //let results = JSON(value)["response"]["results"]
                let results = JSON(value)
                let size = results.count-1
                //print(results[0]["formattedValue"])
                //let size = value.count-1
                for n in 1...size{
                    self.json.append(results[n])
                    
                    //let yVal = results[n]["value"][0]
                    //let insertY = yVal.doubleValue
//                    print("*********")
//                    print(Double(n))
//                    print(results[n]["value"][0].doubleValue)

                    self.yValues.append(ChartDataEntry(x: Double(n), y: results[n]["value"][0].doubleValue))
                }
                SwiftSpinner.hide();
                //print("JSON: \(self.json)")
                self.view.addSubview(self.lineChartView)
                self.lineChartView.centerInSuperview()
                self.lineChartView.width(to: self.view)
                self.lineChartView.heightToWidth(of: self.view)
                
                self.setData()
                case .failure(let error):
                    print(error)
                }
        
                //self.homeTable.reloadData();
        }
        
        
        
    }
    
    func chartValueSelected(_ chartView: ChartViewBase, entry: ChartDataEntry, highlight: Highlight) {
        print(entry)
    }
    
    func setData(){
        print(yValues)
        let set1 = LineChartDataSet(values: yValues, label: "Trending Chart for Coronavirus")
        //let set1 = LineChartDataSet(values: self.json, label: "Trending Chart for Coronavirus")
        let data = LineChartData(dataSet: set1)
        lineChartView.data = data
    }
    
//    let yValues: [ChartDataEntry] = [
//        ChartDataEntry(x: 1.0, y: 5.0),
//        ChartDataEntry(x: 2.0, y: 7.0),
//        ChartDataEntry(x: 3.0, y: 5.0),
//        ChartDataEntry(x: 4.0, y: 10.0),
//        ChartDataEntry(x: 5.0, y:6.0)
//    ]
    
//    var url = "http://localhost:5000/trending/coronavirus";
//
//    override func viewDidLoad(){
//        super.viewDidLoad()
//
//        SwiftSpinner.show("Loading Trending Page...");
//        // Do any additional setup after loading the view, typically from a nib.
//        Alamofire.request(url, method: .get).validate().responseJSON{ response in
//            switch response.result{
//            case .success(let value):
//                let results = JSON(value)["response"]["results"]
//                let size = results.count-1
//                for n in 1...size{
//                    self.json.append(results[n])
//                }
//                SwiftSpinner.hide();
//            //print("JSON: \(self.json)")
//            case .failure(let error):
//                print(error)
//            }
//
//            self.homeTable.reloadData();
//        }
//
//    }
    
////    let googleTrends = require("google-trends-api");
//    override func viewDidLoad() {
//            super.viewDidLoad()
//        googleTrends.apiMethod({keyword:"coronavirus", startTime: new Date("2019-06-01")})
//        .then(function(results)){
//            print(results);
//        }
//        .catch(function(error)){
//            print(error);
//        };
//    }
}
