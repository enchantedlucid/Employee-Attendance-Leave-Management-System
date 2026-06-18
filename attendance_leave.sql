-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jun 18, 2026 at 07:27 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `attendance_leave`
--

-- --------------------------------------------------------

--
-- Table structure for table `admin`
--

CREATE TABLE `admin` (
  `admin_id` varchar(7) NOT NULL,
  `employee_id` varchar(10) NOT NULL,
  `admin_level` enum('super','standard','readonly') DEFAULT 'standard',
  `can_manage_employees` tinyint(1) DEFAULT 0,
  `can_manage_departments` tinyint(1) DEFAULT 0,
  `can_generate_reports` tinyint(1) DEFAULT 0,
  `last_login` datetime DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `created_by` varchar(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `admin`
--

INSERT INTO `admin` (`admin_id`, `employee_id`, `admin_level`, `can_manage_employees`, `can_manage_departments`, `can_generate_reports`, `last_login`, `created_at`, `created_by`) VALUES
('ADM001', 'EMP001', 'super', 1, 1, 1, NULL, '2025-06-11 06:32:45', 'EMP001');

--
-- Triggers `admin`
--
DELIMITER $$
CREATE TRIGGER `generate_admin_id` BEFORE INSERT ON `admin` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('ADM', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(admin_id, 4) AS UNSIGNED)), 0) + 1 FROM Admin), 3, '0'));
    SET NEW.admin_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `attendance`
--

CREATE TABLE `attendance` (
  `attendance_id` varchar(10) NOT NULL,
  `employee_id` varchar(10) NOT NULL,
  `attendance_date` date NOT NULL,
  `clock_in_time` datetime DEFAULT NULL,
  `clock_out_time` datetime DEFAULT NULL,
  `status` enum('Present','Absent','Late','On Leave','Half Day','Overtime') DEFAULT 'Absent',
  `working_hours` decimal(5,2) DEFAULT NULL,
  `remarks` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `attendance`
--

INSERT INTO `attendance` (`attendance_id`, `employee_id`, `attendance_date`, `clock_in_time`, `clock_out_time`, `status`, `working_hours`, `remarks`) VALUES
('ATT001', 'EMP007', '2025-06-11', '2025-06-11 15:34:16', '2025-06-11 15:51:27', 'Half Day', 0.28, 'Worked less than 4 hours'),
('ATT003', 'EMP004', '2025-06-17', '2025-06-17 08:30:00', '2025-06-17 12:00:00', 'Half Day', 3.50, 'Worked less than 4 hours'),
('ATT005', 'EMP006', '2025-06-17', '2025-06-17 09:15:00', '2025-06-17 13:00:00', 'Half Day', 3.75, 'Worked less than 4 hours'),
('ATT006', 'EMP007', '2025-06-17', '2025-06-17 08:10:00', '2025-06-17 17:00:00', 'Present', 8.83, 'Normal working day'),
('ATT008', 'EMP008', '2025-06-18', '2025-06-18 08:15:00', '2025-06-18 17:00:00', 'Present', 8.75, 'Normal working day'),
('ATT009', 'EMP009', '2025-06-18', '2025-06-18 09:20:00', '2025-06-18 13:00:00', 'Half Day', 3.67, 'Worked less than 4 hours'),
('ATT010', 'EMP010', '2025-06-18', '2025-06-18 07:45:00', '2025-06-18 18:30:00', 'Overtime', 10.75, 'Worked more than 9 hours'),
('ATT011', 'EMP012', '2025-06-20', '2025-06-20 11:34:31', '2025-06-20 22:15:28', 'Overtime', 10.67, 'Worked more than 9 hours'),
('ATT012', 'EMP013', '2025-06-19', '2025-06-19 08:00:00', '2025-06-19 17:00:00', 'Present', 9.00, 'Normal day'),
('ATT013', 'EMP014', '2025-06-19', '2025-06-19 08:30:00', '2025-06-19 18:00:00', 'Overtime', 9.50, 'Overtime'),
('ATT014', 'EMP015', '2025-06-19', '2025-06-19 09:00:00', '2025-06-19 13:00:00', 'Half Day', 4.00, 'Short day'),
('ATT015', 'EMP016', '2025-06-19', '2025-06-19 08:15:00', '2025-06-19 17:00:00', 'Present', 8.75, ''),
('ATT016', 'EMP017', '2025-06-19', '2025-06-19 08:45:00', '2025-06-19 18:10:00', 'Overtime', 9.42, 'Long work'),
('ATT017', 'EMP018', '2025-06-19', '2025-06-19 08:20:00', '2025-06-19 17:05:00', 'Present', 8.75, ''),
('ATT018', 'EMP019', '2025-06-19', '2025-06-19 08:30:00', '2025-06-19 17:30:00', 'Present', 9.00, ''),
('ATT019', 'EMP020', '2025-06-19', '2025-06-19 08:00:00', '2025-06-19 13:00:00', 'Half Day', 5.00, ''),
('ATT020', 'EMP021', '2025-06-19', '2025-06-19 08:00:00', '2025-06-19 18:30:00', 'Overtime', 10.50, ''),
('ATT021', 'EMP022', '2025-06-19', '2025-06-19 09:00:00', '2025-06-19 17:00:00', 'Present', 8.00, ''),
('ATT022', 'EMP012', '2025-06-23', '2025-06-23 18:43:22', NULL, 'Present', NULL, NULL),
('ATT023', 'EMP023', '2025-06-24', '2025-06-24 11:17:28', '2025-06-24 12:56:18', 'Half Day', 1.63, 'Worked less than 4 hours'),
('ATT024', 'EMP007', '2025-06-24', '2025-06-24 08:10:00', '2025-06-24 17:00:00', 'Present', 8.83, 'Normal working day'),
('ATT025', 'EMP009', '2025-06-24', '2025-06-24 09:15:00', '2025-06-24 13:00:00', 'Half Day', 3.75, 'Worked less than 4 hours'),
('ATT026', 'EMP015', '2025-06-24', '2025-06-24 08:00:00', '2025-06-24 18:30:00', 'Overtime', 10.50, 'Extended shift'),
('ATT027', 'EMP025', '2025-06-24', '2025-06-24 11:54:00', NULL, 'Present', NULL, NULL),
('ATT029', 'EMP027', '2025-06-27', '2025-06-27 15:23:10', '2025-06-27 15:23:51', 'Half Day', 0.00, 'Worked less than 4 hours'),
('ATT030', 'EMP029', '2026-06-18', '2026-06-18 01:48:14', NULL, 'Present', NULL, NULL),
('ATT031', 'EMP028', '2026-06-18', '2026-06-18 01:55:03', NULL, 'Present', NULL, NULL);

--
-- Triggers `attendance`
--
DELIMITER $$
CREATE TRIGGER `generate_attendance_id` BEFORE INSERT ON `attendance` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('ATT', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(attendance_id, 4) AS UNSIGNED)), 0) + 1 FROM Attendance), 3, '0'));
    SET NEW.attendance_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `department`
--

CREATE TABLE `department` (
  `department_id` varchar(7) NOT NULL,
  `department_name` varchar(100) NOT NULL,
  `description` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `department`
--

INSERT INTO `department` (`department_id`, `department_name`, `description`) VALUES
('DEP001', 'Human Resource', 'Handles employee-related functions'),
('DEP002', 'Finance', 'Manages financial activities'),
('DEP003', 'IT', 'Responsible for system maintenance and support'),
('DEP004', 'Operations', 'Handles logistics and day-to-day activities'),
('DEP005', 'Human Resources', NULL),
('DEP006', 'Audit', ''),
('DEP007', 'Marketing', NULL),
('DEP008', 'IT Support', NULL),
('DEP009', 'Media', 'make a media promotion and deals with media');

--
-- Triggers `department`
--
DELIMITER $$
CREATE TRIGGER `generate_department_id` BEFORE INSERT ON `department` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('DEP', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(department_id, 4) AS UNSIGNED)), 0) + 1 FROM Department), 3, '0'));
    SET NEW.department_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `employee`
--

CREATE TABLE `employee` (
  `employee_id` varchar(10) NOT NULL,
  `username` varchar(50) NOT NULL,
  `employee_name` varchar(50) NOT NULL,
  `contact_number` varchar(15) NOT NULL,
  `email` varchar(100) NOT NULL,
  `password` varchar(255) NOT NULL,
  `department_id` varchar(7) DEFAULT NULL,
  `hire_date` date DEFAULT curdate(),
  `user_type` enum('admin','manager','staff') NOT NULL DEFAULT 'staff',
  `is_active` tinyint(1) DEFAULT 1
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `employee`
--

INSERT INTO `employee` (`employee_id`, `username`, `employee_name`, `contact_number`, `email`, `password`, `department_id`, `hire_date`, `user_type`, `is_active`) VALUES
('EMP001', 'admin1', 'Admin', '0123456789', 'admin1@gmail.com', 'P@ssw0rd123!', NULL, '2025-06-11', 'admin', 1),
('EMP002', 'manager1', 'Maria Manager', '0198765432', 'manager1@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-11', 'manager', 1),
('EMP004', 'zhang', 'zhang lu rang', '0154926481', 'zhang@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-11', 'staff', 1),
('EMP006', 'hadirah', 'hadirah', '0193648256', 'hadirah@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-11', 'staff', 1),
('EMP007', 'haidah', 'haidah', '0158948846', 'haidah@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-11', 'manager', 1),
('EMP008', 'sofia', 'Sofia Rahim', '0192345678', 'sofia@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-11', 'staff', 1),
('EMP009', 'rayyan', 'Rayyan Zaki', '0169876543', 'rayyan@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-11', 'staff', 1),
('EMP010', 'lisa', 'Lisa Ahmad', '0128765432', 'lisa@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-11', 'staff', 1),
('EMP011', 'amir', 'Amir Hamzah', '0123344556', 'amir@gmail.com', 'P@ssw0rd123!', 'DEP004', '2025-06-18', 'manager', 1),
('EMP012', 'fatnin', 'nurul fatnin ', '0195512634', 'fatnin@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-20', 'staff', 1),
('EMP013', 'anisah', 'Anisah Malik', '0173456789', 'anisah@gmail.com', 'P@ssw0rd123!', 'DEP001', '2025-06-20', 'staff', 1),
('EMP014', 'haziq', 'Mohd Haziq', '0139876543', 'haziq@gmail.com', 'P@ssw0rd123!', 'DEP001', '2025-06-20', 'staff', 1),
('EMP015', 'dinah', 'Nur Dinah', '0114567890', 'dinah@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-20', 'staff', 1),
('EMP016', 'ismail', 'Ismail Abu', '0192233445', 'ismail@gmail.com', 'P@ssw0rd123!', 'DEP004', '2025-06-20', 'staff', 1),
('EMP017', 'raihan', 'Raihan Wahid', '0123344556', 'raihan@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-20', 'staff', 1),
('EMP018', 'diyana', 'Diyana Zakaria', '0179876543', 'diyana@gmail.com', 'P@ssw0rd123!', 'DEP004', '2025-06-20', 'staff', 1),
('EMP019', 'syafiq', 'Syafiq Hassan', '0191122334', 'syafiq@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-20', 'staff', 1),
('EMP020', 'najwa', 'Najwa Karim', '0169988776', 'najwa@gmail.com', 'P@ssw0rd123!', 'DEP001', '2025-06-20', 'staff', 1),
('EMP021', 'bryan', 'Bryan Chin', '0144556677', 'bryan@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-20', 'staff', 1),
('EMP022', 'amira', 'Amira Lim', '0126677889', 'amira@gmail.com', 'P@ssw0rd123!', 'DEP002', '2025-06-20', 'staff', 1),
('EMP023', 'amin', 'amin', '0193259366', 'amin@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-24', 'staff', 1),
('EMP024', 'shaqirah', 'shaqirah', '0194852688', 'shaqirah@gmail.com', 'P@ssw0rd123!', 'DEP001', '2025-06-24', 'manager', 1),
('EMP025', 'aminah', 'nur aminah', '0169857263', 'aminah@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-24', 'staff', 1),
('EMP026', 'nurulfatnin', 'NURUL FATNIN', '0193186899', 'nurulfatnin@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-24', 'staff', 1),
('EMP027', 'ali', 'mohd ali', '0169572644', 'ali01@gmail.com', 'P@ssw0rd123!', 'DEP003', '2025-06-27', 'staff', 1),
('EMP028', 'fatihah', 'nurul fatihah', '0185492677', 'fatihah@gmail.com', 'P@ssw0rd123!', 'DEP006', '2026-06-18', 'manager', 1),
('EMP029', 'salsabila', 'salsabila ani', '0185426822', 'salsabila@gmail.com', 'P@ssw0rd123!', 'DEP006', '2026-06-18', 'staff', 1);

--
-- Triggers `employee`
--
DELIMITER $$
CREATE TRIGGER `auto_insert_manager` AFTER INSERT ON `employee` FOR EACH ROW BEGIN
  IF NEW.user_type = 'manager' THEN
    INSERT INTO manager (
      employee_id,
      department_id,
      manager_name
    )
    VALUES (
      NEW.employee_id,
      NEW.department_id,
      NEW.employee_name
    );
  END IF;
END
$$
DELIMITER ;
DELIMITER $$
CREATE TRIGGER `generate_employee_id` BEFORE INSERT ON `employee` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('EMP', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(employee_id, 4) AS UNSIGNED)), 0) + 1 FROM Employee), 3, '0'));
    SET NEW.employee_id = new_id;
END
$$
DELIMITER ;
DELIMITER $$
CREATE TRIGGER `insert_leave_balance_after_employee` AFTER INSERT ON `employee` FOR EACH ROW BEGIN
    DECLARE leave_type VARCHAR(10);
    DECLARE done INT DEFAULT 0;
    DECLARE cur CURSOR FOR SELECT leave_type_id FROM leave_type;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;

    OPEN cur;
    loop1: LOOP
        FETCH cur INTO leave_type;
        IF done THEN
            LEAVE loop1;
        END IF;

        INSERT INTO leave_balance(employee_id, leave_type_id, total_days, used_days, remaining_days, year)
        VALUES (
            NEW.employee_id,
            leave_type,
            (SELECT max_days FROM leave_type WHERE leave_type_id = leave_type),
            0,
            (SELECT max_days FROM leave_type WHERE leave_type_id = leave_type),
            YEAR(CURDATE())
        );
    END LOOP;
    CLOSE cur;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `holiday`
--

CREATE TABLE `holiday` (
  `holiday_id` varchar(10) NOT NULL,
  `holiday_name` varchar(100) NOT NULL,
  `holiday_date` date NOT NULL,
  `description` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `holiday`
--

INSERT INTO `holiday` (`holiday_id`, `holiday_name`, `holiday_date`, `description`) VALUES
('HOL001', 'New Year', '2025-01-01', 'First day of the year'),
('HOL002', 'Chinese New Year', '2025-02-01', 'Lunar New Year celebration'),
('HOL003', 'Hari Raya Aidilfitri', '2025-04-10', 'End of Ramadan'),
('HOL004', 'Hari Merdeka', '2025-08-31', 'Malaysia Independence Day'),
('HOL005', 'Deepavali', '2025-11-01', 'Festival of Lights'),
('HOL006', 'Christmas Day', '2025-12-25', 'Celebration of Christmas'),
('HOL007', 'Federal Territory Day', '2025-02-10', 'Holiday for FT regions'),
('HOL008', 'Labour Day', '2025-05-01', 'International Workers’ Day'),
('HOL009', 'Agong’s Birthday', '2025-06-01', 'Birthday of the Yang di-Pertuan Agong'),
('HOL010', 'Malaysia Day', '2025-09-16', 'Formation of Malaysia');

--
-- Triggers `holiday`
--
DELIMITER $$
CREATE TRIGGER `generate_holiday_id` BEFORE INSERT ON `holiday` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('HOL', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(holiday_id, 4) AS UNSIGNED)), 0) + 1 FROM Holiday), 3, '0'));
    SET NEW.holiday_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `leave_balance`
--

CREATE TABLE `leave_balance` (
  `balance_id` varchar(10) NOT NULL,
  `employee_id` varchar(10) NOT NULL,
  `leave_type_id` varchar(10) NOT NULL,
  `total_days` decimal(5,2) DEFAULT 30.00,
  `used_days` decimal(5,2) DEFAULT 0.00,
  `remaining_days` decimal(5,2) DEFAULT 30.00,
  `year` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `leave_balance`
--

INSERT INTO `leave_balance` (`balance_id`, `employee_id`, `leave_type_id`, `total_days`, `used_days`, `remaining_days`, `year`) VALUES
('LBL007', 'EMP004', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL008', 'EMP004', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL009', 'EMP004', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL010', 'EMP004', 'LT005', 7.00, 2.00, 5.00, 2025),
('LBL011', 'EMP004', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL012', 'EMP004', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL019', 'EMP006', 'LT001', 14.00, 1.00, 13.00, 2025),
('LBL020', 'EMP006', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL021', 'EMP006', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL022', 'EMP006', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL023', 'EMP006', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL024', 'EMP006', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL025', 'EMP007', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL026', 'EMP007', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL027', 'EMP007', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL028', 'EMP007', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL029', 'EMP007', 'LT002', 14.00, 2.00, 12.00, 2025),
('LBL030', 'EMP007', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL031', 'EMP001', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL032', 'EMP008', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL033', 'EMP008', 'LT003', 7.00, 3.00, 4.00, 2025),
('LBL034', 'EMP008', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL035', 'EMP008', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL036', 'EMP008', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL037', 'EMP008', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL038', 'EMP009', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL039', 'EMP009', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL040', 'EMP009', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL041', 'EMP009', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL042', 'EMP009', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL043', 'EMP009', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL044', 'EMP010', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL045', 'EMP010', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL046', 'EMP010', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL047', 'EMP010', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL048', 'EMP010', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL049', 'EMP010', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL050', 'EMP011', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL051', 'EMP011', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL052', 'EMP011', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL053', 'EMP011', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL054', 'EMP011', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL055', 'EMP011', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL056', 'EMP012', 'LT001', 14.00, 4.00, 10.00, 2025),
('LBL057', 'EMP012', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL058', 'EMP012', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL059', 'EMP012', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL060', 'EMP012', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL061', 'EMP012', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL062', 'EMP013', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL063', 'EMP013', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL064', 'EMP013', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL065', 'EMP013', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL066', 'EMP013', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL067', 'EMP013', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL068', 'EMP014', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL069', 'EMP014', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL070', 'EMP014', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL071', 'EMP014', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL072', 'EMP014', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL073', 'EMP014', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL074', 'EMP015', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL075', 'EMP015', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL076', 'EMP015', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL077', 'EMP015', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL078', 'EMP015', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL079', 'EMP015', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL080', 'EMP016', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL081', 'EMP016', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL082', 'EMP016', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL083', 'EMP016', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL084', 'EMP016', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL085', 'EMP016', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL086', 'EMP017', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL087', 'EMP017', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL088', 'EMP017', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL089', 'EMP017', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL090', 'EMP017', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL091', 'EMP017', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL092', 'EMP018', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL093', 'EMP018', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL094', 'EMP018', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL095', 'EMP018', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL096', 'EMP018', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL097', 'EMP018', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL098', 'EMP019', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL099', 'EMP019', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL100', 'EMP019', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL101', 'EMP019', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL102', 'EMP019', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL103', 'EMP019', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL104', 'EMP020', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL105', 'EMP020', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL106', 'EMP020', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL107', 'EMP020', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL108', 'EMP020', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL109', 'EMP020', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL110', 'EMP021', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL111', 'EMP021', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL112', 'EMP021', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL113', 'EMP021', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL114', 'EMP021', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL115', 'EMP021', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL116', 'EMP022', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL117', 'EMP022', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL118', 'EMP022', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL119', 'EMP022', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL120', 'EMP022', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL121', 'EMP022', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL122', 'EMP013', 'LT001', 14.00, 2.00, 12.00, 2025),
('LBL123', 'EMP014', 'LT002', 10.00, 1.00, 9.00, 2025),
('LBL124', 'EMP015', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL125', 'EMP016', 'LT003', 7.00, 1.00, 6.00, 2025),
('LBL126', 'EMP017', 'LT001', 14.00, 1.00, 13.00, 2025),
('LBL127', 'EMP018', 'LT002', 10.00, 0.00, 10.00, 2025),
('LBL128', 'EMP019', 'LT003', 7.00, 1.00, 6.00, 2025),
('LBL129', 'EMP020', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL130', 'EMP021', 'LT005', 3.00, 3.00, 0.00, 2025),
('LBL131', 'EMP022', 'LT002', 10.00, 0.00, 10.00, 2025),
('LBL132', 'EMP023', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL133', 'EMP023', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL134', 'EMP023', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL135', 'EMP023', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL136', 'EMP023', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL137', 'EMP023', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL138', 'EMP024', 'LT001', 14.00, 0.00, 14.00, 2025),
('LBL139', 'EMP024', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL140', 'EMP024', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL141', 'EMP024', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL142', 'EMP024', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL143', 'EMP024', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL144', 'EMP025', 'LT001', 14.00, 3.00, 11.00, 2025),
('LBL145', 'EMP025', 'LT002', 14.00, 1.00, 13.00, 2025),
('LBL146', 'EMP025', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL147', 'EMP025', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL148', 'EMP025', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL149', 'EMP025', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL150', 'EMP026', 'LT001', 14.00, 3.00, 11.00, 2025),
('LBL151', 'EMP026', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL152', 'EMP026', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL153', 'EMP026', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL154', 'EMP026', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL155', 'EMP026', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL156', 'EMP027', 'LT001', 14.00, 5.00, 9.00, 2025),
('LBL157', 'EMP027', 'LT003', 7.00, 0.00, 7.00, 2025),
('LBL158', 'EMP027', 'LT004', 60.00, 0.00, 60.00, 2025),
('LBL159', 'EMP027', 'LT005', 7.00, 0.00, 7.00, 2025),
('LBL160', 'EMP027', 'LT002', 14.00, 0.00, 14.00, 2025),
('LBL161', 'EMP027', 'LT006', 0.00, 0.00, 0.00, 2025),
('LBL162', 'EMP028', 'LT001', 14.00, 0.00, 14.00, 2026),
('LBL163', 'EMP028', 'LT002', 14.00, 0.00, 14.00, 2026),
('LBL164', 'EMP028', 'LT003', 7.00, 0.00, 7.00, 2026),
('LBL165', 'EMP028', 'LT004', 60.00, 0.00, 60.00, 2026),
('LBL166', 'EMP028', 'LT005', 7.00, 0.00, 7.00, 2026),
('LBL167', 'EMP028', 'LT006', 0.00, 0.00, 0.00, 2026),
('LBL168', 'EMP029', 'LT001', 14.00, 3.00, 11.00, 2026),
('LBL169', 'EMP029', 'LT002', 14.00, 0.00, 14.00, 2026),
('LBL170', 'EMP029', 'LT003', 7.00, 0.00, 7.00, 2026),
('LBL171', 'EMP029', 'LT004', 60.00, 0.00, 60.00, 2026),
('LBL172', 'EMP029', 'LT005', 7.00, 0.00, 7.00, 2026),
('LBL173', 'EMP029', 'LT006', 0.00, 0.00, 0.00, 2026);

--
-- Triggers `leave_balance`
--
DELIMITER $$
CREATE TRIGGER `generate_balance_id` BEFORE INSERT ON `leave_balance` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('LBL', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(balance_id, 4) AS UNSIGNED)), 0) + 1 FROM Leave_Balance), 3, '0'));
    SET NEW.balance_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `leave_log`
--

CREATE TABLE `leave_log` (
  `log_id` int(11) NOT NULL,
  `employee_id` varchar(10) DEFAULT NULL,
  `leave_type_id` varchar(10) DEFAULT NULL,
  `days_taken` int(11) DEFAULT NULL,
  `approved_by` varchar(10) DEFAULT NULL,
  `approved_date` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `leave_log`
--

INSERT INTO `leave_log` (`log_id`, `employee_id`, `leave_type_id`, `days_taken`, `approved_by`, `approved_date`) VALUES
(2, 'EMP005', 'LT002', 1, 'EMP002', '2025-06-15'),
(3, 'EMP008', 'LT003', 3, 'EMP002', '2025-06-17'),
(4, 'EMP007', 'LT001', 1, 'EMP002', '2025-06-11'),
(6, 'EMP005', 'LT002', 1, 'EMP002', '2025-06-15'),
(7, 'EMP008', 'LT003', 3, 'EMP002', '2025-06-17'),
(8, 'EMP004', 'LT005', 2, 'EMP003', '2025-06-18'),
(9, 'EMP006', 'LT001', 1, 'EMP003', '2025-06-18'),
(10, 'EMP007', 'LT002', 2, 'EMP003', '2025-06-18'),
(11, 'EMP013', 'LT001', 2, 'EMP002', '2025-07-02'),
(12, 'EMP014', 'LT002', 1, 'EMP003', '2025-07-03'),
(13, 'EMP017', 'LT001', 1, 'EMP003', '2025-07-06'),
(14, 'EMP019', 'LT003', 1, 'EMP007', '2025-07-09'),
(15, 'EMP021', 'LT005', 3, 'EMP002', '2025-07-13');

-- --------------------------------------------------------

--
-- Table structure for table `leave_request`
--

CREATE TABLE `leave_request` (
  `leave_request_id` varchar(10) NOT NULL,
  `employee_id` varchar(10) NOT NULL,
  `leave_type_id` varchar(10) NOT NULL,
  `start_date` date NOT NULL,
  `end_date` date NOT NULL,
  `reason` text DEFAULT NULL,
  `request_date` timestamp NOT NULL DEFAULT current_timestamp(),
  `status` enum('Pending','Approved','Rejected','Cancelled') DEFAULT 'Pending',
  `approved_by` varchar(10) DEFAULT NULL,
  `remarks` text DEFAULT NULL,
  `approved_date` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `leave_request`
--

INSERT INTO `leave_request` (`leave_request_id`, `employee_id`, `leave_type_id`, `start_date`, `end_date`, `reason`, `request_date`, `status`, `approved_by`, `remarks`, `approved_date`) VALUES
('LRE001', 'EMP007', 'LT001', '2025-06-12', '2025-06-12', 'had an appointment with dr', '2025-06-11 07:35:13', 'Approved', 'EMP002', 'Approved by Maria', NULL),
('LRE003', 'EMP004', 'LT005', '2025-06-23', '2025-06-24', 'Paternity leave', '2025-06-18 01:10:00', 'Approved', 'EMP003', NULL, NULL),
('LRE005', 'EMP006', 'LT003', '2025-06-26', '2025-06-28', 'Urgent matter', '2025-06-18 02:00:00', 'Pending', NULL, NULL, NULL),
('LRE006', 'EMP008', 'LT004', '2025-06-29', '2025-07-02', 'Maternity', '2025-06-18 02:30:00', 'Approved', 'EMP002', NULL, NULL),
('LRE007', 'EMP012', 'LT001', '2025-07-25', '2025-07-28', 'going on a vacation with family', '2025-06-20 03:35:47', '', 'EMP007', NULL, NULL),
('LRE008', 'EMP007', 'LT002', '2025-06-20', '2025-06-20', 'having a fever', '2025-06-20 03:52:25', 'Pending', NULL, NULL, NULL),
('LRE009', 'EMP013', 'LT001', '2025-07-01', '2025-07-02', 'Wedding leave', '2025-06-20 13:35:29', 'Approved', 'EMP002', NULL, NULL),
('LRE010', 'EMP014', 'LT002', '2025-07-03', '2025-07-03', 'Flu and rest', '2025-06-20 13:35:29', 'Approved', 'EMP003', NULL, NULL),
('LRE011', 'EMP015', 'LT001', '2025-07-04', '2025-07-04', 'Personal errands', '2025-06-20 13:35:29', 'Pending', NULL, NULL, NULL),
('LRE012', 'EMP016', 'LT003', '2025-07-05', '2025-07-05', 'Emergency leave', '2025-06-20 13:35:29', 'Rejected', 'EMP007', NULL, NULL),
('LRE013', 'EMP017', 'LT001', '2025-07-06', '2025-07-06', 'One day trip', '2025-06-20 13:35:29', 'Approved', 'EMP003', NULL, NULL),
('LRE014', 'EMP018', 'LT002', '2025-07-07', '2025-07-08', 'Medical rest', '2025-06-20 13:35:29', 'Pending', NULL, NULL, NULL),
('LRE015', 'EMP019', 'LT003', '2025-07-09', '2025-07-09', 'Family emergency', '2025-06-20 13:35:29', 'Approved', 'EMP007', NULL, NULL),
('LRE016', 'EMP020', 'LT001', '2025-07-10', '2025-07-10', 'Mental wellness leave', '2025-06-20 13:35:29', 'Pending', NULL, NULL, NULL),
('LRE017', 'EMP021', 'LT005', '2025-07-11', '2025-07-13', 'Paternity', '2025-06-20 13:35:29', 'Approved', 'EMP002', NULL, NULL),
('LRE018', 'EMP022', 'LT002', '2025-07-14', '2025-07-14', 'Migraine', '2025-06-20 13:35:29', 'Pending', NULL, NULL, NULL),
('LRE020', 'EMP025', 'LT001', '2025-06-30', '2025-07-02', 'going to europe', '2025-06-24 03:57:15', '', 'EMP007', NULL, NULL),
('LRE021', 'EMP023', 'LT002', '2025-06-26', '2025-06-26', 'going on a check-up at hospital', '2025-06-24 04:57:04', 'Approved', 'EMP001', NULL, '2026-06-18'),
('LRE022', 'EMP026', 'LT001', '2025-07-28', '2025-07-30', 'going to a vacation', '2025-06-24 06:12:27', '', 'EMP007', NULL, NULL),
('LRE023', 'EMP025', 'LT002', '2025-06-24', '2025-06-24', 'sick', '2025-06-24 06:17:22', '', 'EMP007', NULL, NULL),
('LRE024', 'EMP027', 'LT001', '2025-07-10', '2025-07-14', 'going to vacation', '2025-06-27 07:25:07', '', 'EMP007', NULL, NULL),
('LRE025', 'EMP029', 'LT001', '2026-07-22', '2026-07-24', 'went on vacation trip', '2026-06-17 17:49:13', '', 'EMP028', NULL, NULL);

--
-- Triggers `leave_request`
--
DELIMITER $$
CREATE TRIGGER `generate_leave_request_id` BEFORE INSERT ON `leave_request` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('LRE', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(leave_request_id, 4) AS UNSIGNED)), 0) + 1 FROM Leave_Request), 3, '0'));
    SET NEW.leave_request_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `leave_type`
--

CREATE TABLE `leave_type` (
  `leave_type_id` varchar(10) NOT NULL,
  `type_name` varchar(50) NOT NULL,
  `description` text DEFAULT NULL,
  `max_days` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `leave_type`
--

INSERT INTO `leave_type` (`leave_type_id`, `type_name`, `description`, `max_days`) VALUES
('LT001', 'Annual Leave', 'Yearly leave entitlement', 14),
('LT002', 'Sick Leave', 'Medical or health-related leave', 14),
('LT003', 'Emergency Leave', 'Urgent personal reasons', 7),
('LT004', 'Maternity Leave', 'For female employees giving birth', 60),
('LT005', 'Paternity Leave', 'For male employees whose wife gives birth', 7),
('LT006', 'Unpaid Leave', 'No pay granted leave', 0);

--
-- Triggers `leave_type`
--
DELIMITER $$
CREATE TRIGGER `generate_leave_type_id` BEFORE INSERT ON `leave_type` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('LT', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(leave_type_id, 3) AS UNSIGNED)), 0) + 1 FROM Leave_Type), 3, '0'));
    SET NEW.leave_type_id = new_id;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `manager`
--

CREATE TABLE `manager` (
  `manager_id` varchar(7) NOT NULL,
  `employee_id` varchar(10) DEFAULT NULL,
  `department_id` varchar(7) NOT NULL,
  `manager_name` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `manager`
--

INSERT INTO `manager` (`manager_id`, `employee_id`, `department_id`, `manager_name`) VALUES
('MGR001', 'EMP002', 'DEP002', 'Maria Manager'),
('MGR002', 'EMP011', 'DEP004', 'Amir Hamzah'),
('MGR004', 'EMP007', 'DEP003', 'haidah'),
('MGR005', 'EMP024', 'DEP001', 'shaqirah'),
('MGR006', 'EMP028', 'DEP006', 'nurul fatihah'),
('MGR007', 'EMP028', 'DEP006', 'nurul fatihah');

--
-- Triggers `manager`
--
DELIMITER $$
CREATE TRIGGER `generate_manager_id` BEFORE INSERT ON `manager` FOR EACH ROW BEGIN
    DECLARE new_id VARCHAR(10);
    SET new_id = CONCAT('MGR', LPAD((SELECT IFNULL(MAX(CAST(SUBSTRING(manager_id, 4) AS UNSIGNED)), 0) + 1 FROM Manager), 3, '0'));
    SET NEW.manager_id = new_id;
END
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `admin`
--
ALTER TABLE `admin`
  ADD PRIMARY KEY (`admin_id`),
  ADD KEY `employee_id` (`employee_id`),
  ADD KEY `created_by` (`created_by`);

--
-- Indexes for table `attendance`
--
ALTER TABLE `attendance`
  ADD PRIMARY KEY (`attendance_id`),
  ADD KEY `employee_id` (`employee_id`);

--
-- Indexes for table `department`
--
ALTER TABLE `department`
  ADD PRIMARY KEY (`department_id`);

--
-- Indexes for table `employee`
--
ALTER TABLE `employee`
  ADD PRIMARY KEY (`employee_id`),
  ADD UNIQUE KEY `username` (`username`),
  ADD UNIQUE KEY `email` (`email`),
  ADD KEY `department_id` (`department_id`);

--
-- Indexes for table `holiday`
--
ALTER TABLE `holiday`
  ADD PRIMARY KEY (`holiday_id`);

--
-- Indexes for table `leave_balance`
--
ALTER TABLE `leave_balance`
  ADD PRIMARY KEY (`balance_id`),
  ADD KEY `employee_id` (`employee_id`),
  ADD KEY `leave_type_id` (`leave_type_id`);

--
-- Indexes for table `leave_log`
--
ALTER TABLE `leave_log`
  ADD PRIMARY KEY (`log_id`);

--
-- Indexes for table `leave_request`
--
ALTER TABLE `leave_request`
  ADD PRIMARY KEY (`leave_request_id`),
  ADD KEY `employee_id` (`employee_id`),
  ADD KEY `leave_type_id` (`leave_type_id`);

--
-- Indexes for table `leave_type`
--
ALTER TABLE `leave_type`
  ADD PRIMARY KEY (`leave_type_id`),
  ADD UNIQUE KEY `type_name` (`type_name`);

--
-- Indexes for table `manager`
--
ALTER TABLE `manager`
  ADD PRIMARY KEY (`manager_id`),
  ADD KEY `employee_id` (`employee_id`),
  ADD KEY `department_id` (`department_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `leave_log`
--
ALTER TABLE `leave_log`
  MODIFY `log_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=16;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `admin`
--
ALTER TABLE `admin`
  ADD CONSTRAINT `admin_ibfk_1` FOREIGN KEY (`employee_id`) REFERENCES `employee` (`employee_id`),
  ADD CONSTRAINT `admin_ibfk_2` FOREIGN KEY (`created_by`) REFERENCES `employee` (`employee_id`);

--
-- Constraints for table `attendance`
--
ALTER TABLE `attendance`
  ADD CONSTRAINT `attendance_ibfk_1` FOREIGN KEY (`employee_id`) REFERENCES `employee` (`employee_id`);

--
-- Constraints for table `employee`
--
ALTER TABLE `employee`
  ADD CONSTRAINT `employee_ibfk_1` FOREIGN KEY (`department_id`) REFERENCES `department` (`department_id`);

--
-- Constraints for table `leave_balance`
--
ALTER TABLE `leave_balance`
  ADD CONSTRAINT `leave_balance_ibfk_1` FOREIGN KEY (`employee_id`) REFERENCES `employee` (`employee_id`),
  ADD CONSTRAINT `leave_balance_ibfk_2` FOREIGN KEY (`leave_type_id`) REFERENCES `leave_type` (`leave_type_id`);

--
-- Constraints for table `leave_request`
--
ALTER TABLE `leave_request`
  ADD CONSTRAINT `leave_request_ibfk_1` FOREIGN KEY (`employee_id`) REFERENCES `employee` (`employee_id`),
  ADD CONSTRAINT `leave_request_ibfk_2` FOREIGN KEY (`leave_type_id`) REFERENCES `leave_type` (`leave_type_id`);

--
-- Constraints for table `manager`
--
ALTER TABLE `manager`
  ADD CONSTRAINT `manager_ibfk_1` FOREIGN KEY (`employee_id`) REFERENCES `employee` (`employee_id`),
  ADD CONSTRAINT `manager_ibfk_2` FOREIGN KEY (`department_id`) REFERENCES `department` (`department_id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
