/*
Helper functions for GUI
by: Connor Douthat
10/23/2015
*/
void ErrorBox(const char *message)
{
	MessageBox(hwndMain, message, "Error - pwcliptray", 0);
}
