/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

 /*Creating the Table*/
 CREATE Table HotelStays(
 	roomNum INTEGER NOT NULL,
 	dateOfStay DATE NOT NULL,
 	guestName CHAR(30) NOT NULL,
 	PRIMARY KEY (roomNum, dateOfStay)
 );

 /*How would you redesign the table to fix both issues?*/

 /*
  * The way I solved this issue was by simply deleting
  * the arrival date and departure date attributes. Then,
  * I just create a date of stay to indicate just the day
  * that the person will be staying in the hotel. Thus,
  * Automatically solving both issues because we are not
  * concerned about efficiency. Another approach that is
  * more complicated is using a CHECK to validate that
  * the departure date is later than the arrival date.
  * and create a trigger to make sure that the dates of
  * of reservations for the same room do not overlap.
  * However, this solution involves more programming.
  */