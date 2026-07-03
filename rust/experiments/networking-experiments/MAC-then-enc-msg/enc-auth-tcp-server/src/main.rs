use sodiumoxide::crypto::{box_, sign};
use std::io::{Read, Write};
use std::net::{Shutdown, TcpListener, TcpStream};
use std::thread;
use std::str;

fn handle_client(mut stream: TcpStream) {
    match sodiumoxide::init() {
        Ok(()) => println!("sodium is available for use"),
        Err(_) => println!("Init failed"),
    };
    let (server_enc_pk, server_enc_sk) = box_::gen_keypair();
    let (server_sig_pk, _server_sig_sk) = sign::gen_keypair();
    stream.write(server_enc_pk.as_ref()).unwrap();
    stream.write(server_sig_pk.as_ref()).unwrap();

    let mut nonce = [0 as u8; 24];
    stream
        .read(&mut nonce)
        .expect("something went wrong reading the nonce");
    let mut client_enc_pk = [0 as u8; 32];
    stream
        .read(&mut client_enc_pk)
        .expect("something went wrong reading the client enc pk");
    let mut client_sig_pk = [0 as u8; 32];
    stream
        .read(&mut client_sig_pk)
        .expect("something went wrong reading the client sig pk");
    let mut signed_data = [0 as u8; 93];
    while match stream.read(&mut signed_data) {
        Ok(size) => {
            println!("We received {} bytes of signed data.", size);
            let verified_data = sign::verify(
                &signed_data,
                &sign::ed25519::PublicKey::from_slice(&client_sig_pk).unwrap(),
            )
            .unwrap();
            let decrypted_data = box_::open(
                &verified_data,
                &box_::curve25519xsalsa20poly1305::Nonce::from_slice(&nonce).unwrap(),
                &box_::curve25519xsalsa20poly1305::PublicKey::from_slice(&client_enc_pk).unwrap(),
                &server_enc_sk,
            )
            .unwrap();
            let msg = match str::from_utf8(decrypted_data.as_slice()) {
                Ok(v) => v,
                Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
            };

            println!("{}", msg);
            let resp = b"ACK Client: Hello from the server!";
            stream.write(&resp[0..]).unwrap();
            false
        }
        Err(_) => {
            println!(
                "An error occurred, terminating connection with {}",
                stream.peer_addr().unwrap()
            );
            stream.shutdown(Shutdown::Both).unwrap();
            false
        }
    } {}
}

fn main() {
    let listener = TcpListener::bind("0.0.0.0:3333").unwrap();
    // accept connections and process them, spawning a new thread for each one
    println!("Server listening on port 3333");
    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                println!("New connection {}", stream.peer_addr().unwrap());
                thread::spawn(move || {
                    // connection succeeded
                    handle_client(stream)
                });
            }
            Err(e) => {
                // connection failed
                println!("Error {}", e);
            }
        }
    }
    // close the socket server
    drop(listener);
}
