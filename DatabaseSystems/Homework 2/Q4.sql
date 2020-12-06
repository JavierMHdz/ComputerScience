/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

/*Creating the Table*/
Create Table ProblemFour( 
    Name varchar(255), 
    Address varchar(255), 
    ID int, 
    SameFam int 
);

/*Data Insertion*/
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Alice','A',10,NULL);
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Bob','B',15,NULL);
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Carmen','C',22,NULL);
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Diego','A',9,10);
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Ella','B',3,15);
INSERT INTO ProblemFour(Name, Address, ID, SameFam) Values('Farkhad','D',11,NULL);

/*Query Commands*/
/*
 * In this query I am deleting the rows of table ProblemFour where SameFam is equal
 * to null and the ID of this element is stored in a different element row as
 * SameFam. This means that it has another relative in the table. I am using a subquery
 * to query recursively the same table, similar to having two loops.
 */
DELETE FROM ProblemFour TOne  
WHERE SameFam IS NULL AND EXISTS (SELECT *  
                                  FROM ProblemFour TTwo  
                                  WHERE TTwo.SameFam IS NOT NULL AND TOne.ID = TTwo.SameFam);