use sodiumoxide::crypto::{box_, sign};
use std::io::{Read, Write};
use std::net::TcpStream;
use std::str;

fn main() {
    match sodiumoxide::init() {
        Ok(()) => println!("sodium is available for use"),
        Err(_) => println!("Init failed"),
    };
    let (client_enc_pk, client_enc_sk) = box_::gen_keypair();
    let (client_sig_pk, client_sig_sk) = sign::gen_keypair();
    match TcpStream::connect("localhost:3333") {
        Ok(mut stream) => {
            println!("Successfully connected to server in port 3333");
            let mut server_enc_pk = [0 as u8; 32];
            stream.read(&mut server_enc_pk).unwrap();
            let mut server_sig_pk = [0 as u8; 32];
            stream.read(&mut server_sig_pk).unwrap();
            let nonce = [0 as u8; 24];
            stream.write(&nonce).unwrap();
            stream.write(client_enc_pk.as_ref()).unwrap();
            stream.write(client_sig_pk.as_ref()).unwrap();

            let msg = b"Hello server!";
            let encrypted_msg = box_::seal(
                msg,
                &box_::curve25519xsalsa20poly1305::Nonce::from_slice(&nonce).unwrap(),
                &box_::curve25519xsalsa20poly1305::PublicKey::from_slice(&server_enc_pk).unwrap(),
                &client_enc_sk,
            );
            let maced_encrypted_msg = sign::sign(encrypted_msg.as_slice(), &client_sig_sk);
            stream.write(&maced_encrypted_msg).unwrap();
            println!("Sent Message \"{}\", awaiting reply...", str::from_utf8(msg).unwrap());
            let mut data = [0 as u8; 34];
            let expected_resp = b"ACK Client: Hello from the server!";

            match stream.read_exact(&mut data) {
                Ok(_) => {
                    if &data[..] == expected_resp {
                        println!("{}", str::from_utf8(&data).unwrap());
                    } else {
                        let text = str::from_utf8(&data).unwrap();
                        println!("Unexpected reply: !{}!", text);
                    }
                }
                Err(e) => {
                    println!("Failed to receive data: {}", e);
                }
            }
        }
        Err(e) => {
            println!("Failed to connect: {}", e);
        }
    }
    println!("Terminated.");
}
