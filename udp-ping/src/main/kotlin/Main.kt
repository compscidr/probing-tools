import java.net.InetSocketAddress
import java.nio.channels.DatagramChannel

fun main(args: Array<String>) {
    val datagramChannel = DatagramChannel.open()
    datagramChannel.connect(InetSocketAddress("google.com", 35544))
    println("CONNECTED? " + datagramChannel.isConnected)
}