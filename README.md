# WinPrint
I needed a simple way to print txt files behind the scenes for the Windows-based POS system I am building. My program produces receipts and order-slips as txt files that WinPrint sends to a physical printer. Examples were hard to come by, so I hope this will help others. I've commented the files thoroughly to outline each step involved.

Program assumptions:
- Windows 2000+
- Using the system's default printer (The printer that is chosen automatically upon a manual 'Ctrl-P' print)
- Basic txt files only. The program reads in lines of the file and prints them, with no regard for formatting at this time.
- Currently assumes 1 page, as it suited my purposes.
