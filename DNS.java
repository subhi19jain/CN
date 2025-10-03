import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Scanner;

public class DNS {
public static void main(String[] args) {
Scanner sc = new Scanner(System.in);
System.out.println("DNS Lookup Tool");
System.out.println("1. Enter Domain Name to get IP Address");
System.out.println("2. Enter IP Address to get Domain Name");
System.out.print("Choose option (1/2): ");
int choice = sc.nextInt();
sc.nextLine();  // consume newline


    try {
        if (choice == 1) {
            System.out.print("Enter Domain Name (e.g., www.google.com): ");
            String domain = sc.nextLine();
            InetAddress inetAddress = InetAddress.getByName(domain);
            System.out.println("IP Address: " + inetAddress.getHostAddress());
        } else if (choice == 2) {
            System.out.print("Enter IP Address (e.g., 8.8.8.8): ");
            String ip = sc.nextLine();
            InetAddress inetAddress = InetAddress.getByName(ip);
            System.out.println("Domain Name: " + inetAddress.getHostName());
        } else {
            System.out.println("Invalid choice.");
        }
    } catch (UnknownHostException e) {
        System.out.println("Lookup failed: " + e.getMessage());
    }

    sc.close();
}


}
