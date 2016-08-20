CREATE DEFINER=`root`@`localhost` PROCEDURE `AddUser`(IN iUserId VARCHAR(30), IN iUserPw VARCHAR(30))
BEGIN
	INSERT INTO Users 
    (UserId, UserPw)
    VALUES
    (iUserId, iUserPw);
END