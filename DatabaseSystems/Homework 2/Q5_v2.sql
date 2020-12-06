/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

/*Creating the Table*/
CREATE TABLE ProblemFive( 
    Chef varchar(255), 
    Dish varchar(255) 
);

/*Data Insertion*/
INSERT INTO ProblemFive (Chef, Dish) VALUES ('A','Mint chocolate brownie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Mint chocolate brownie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('C','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('C','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Apple pie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Upside down pineapple cake'); 
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Apple pie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Bananas Foster');

/*Query Commands*/
/*
 * In this query, I choose the chefs that can prepare the three dishes by using
 * subqueries. The inner-most query returns the chefs that can prepare 'creme brulee.'
 * Then, the inner query at the next level returns the chefs that can prepare
 * 'Upside down pineapple cake' and can prepare 'creme brulee,' since the chefs
 * that can prepare 'creme brulee' are part of the condition, the options of chef
 * start to narrow down in that subquery. Finally, the same logic applies to the 
 * outer-most query, which is that it returns the chefs that can make the three dishes.
 * This query should check less amoun of chefs because the options of chefs have
 * already been narrowed down by the previous two subqueries.
 */
SELECT Chef  
FROM ProblemFive T1  
WHERE T1.Dish = 'Apple pie' AND EXISTS (SELECT Chef  
                                        FROM ProblemFive T2  
                                        WHERE T2.Dish = 'Upside down pineapple cake' AND EXISTS (SELECT Chef  
                                                                                                 FROM ProblemFive T3  
                                                                                                 WHERE T3.Dish = 'Creme brulee'));