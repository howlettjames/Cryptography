const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const passport = require('passport');
const { ensureAuthenticated } = require('../config/auth');
const nodemailer = require('nodemailer');
const crypto = require('crypto');
const fs = require('fs');

process.env.NODE_TLS_REJECT_UNAUTHORIZED = "0";

// User model
const User = require("../models/User");

// ---------------------------------------------- TEMPLATES ---------------------------------------
// Index page
router.get('/', (req, res) => res.render('index.ejs'));
router.get('/index', (req, res) => res.render('index.ejs'));

// Create Account page
router.get('/createaccount', (req, res) => res.render('createaccount.ejs'));

// Login page
router.get('/login', (req, res) => res.render('login.ejs'));

// Memo page
router.get('/memo', (req, res) => 
{
    User.find({  })
    .then(users =>
    {
        res.render('memo.ejs', { users: users });
    });
});

// ---------------------------------------------- API ---------------------------------------
// Create an Account
router.post('/createaccount', (req, res) => 
{
    const { firstname, lastname, username, password, passwordconfirm, email, type } = req.body;
    let errors = [];

    // Check required fields
    if(!firstname || !lastname || !username || !password || !passwordconfirm || !email || !type)
        errors.push({ msg: 'Please fill in all fields' });

    // Check passwords match
    if(password !== passwordconfirm)
        errors.push({ msg: 'Passwords do not match' });

    // Check pass length
    if(password.length < 6)
        errors.push({ msg: 'Password should be at least 6 characters long' })

    if(errors.length > 0)
    {
        res.render('createaccount.ejs', 
        {
            errors,
            firstname,
            lastname,
            username,
            password,
            passwordconfirm,
            email
        });
    }
    else
    {
        // Validate passed
        User.findOne({ email: email })
            .then(user =>
                {
                    if(user)
                    {
                        //User exists
                        errors.push({ msg: 'Email is already registered' });
                        res.render('createaccount.ejs', 
                        {
                            errors,
                            firstname,
                            lastname,
                            username,
                            password,
                            passwordconfirm,
                            email
                        });
                    }
                    else
                    {
                        const { publicKey, privateKey } = crypto.generateKeyPairSync('rsa', 
                        {
                            modulusLength: 4096,
                            publicKeyEncoding:
                            {
                                type: 'pkcs1',
                                format: 'pem'
                            },
                            privateKeyEncoding:
                            {
                                    type: 'pkcs1',
                                    format: 'pem',
                                    // cipher: 'aes-256-cbc',
                                    // passphrase: 'top secret'
                            }
                        });

                        const newUser = new User
                        ({ 
                            firstname,
                            lastname,
                            username,
                            password,
                            passwordconfirm,
                            publickey: publicKey,
                            email,
                            type
                        });

                        fs.writeFile(username + '_privatekey.txt', privateKey, (err) =>
                        {
                            if(err) throw err;
                            console.log('Private Key Saved!');
                        });

                        fs.writeFile(username + '_publickey.txt', publicKey, (err) =>
                        {
                            if(err) throw err;
                            console.log('Public Key Saved!');
                        });
                        // var transporter = nodemailer.createTransport
                        // ({
                        //     service: 'gmail',
                        //     auth: 
                        //     {
                        //         user: 'jamesf6888@gmail.com',
                        //         pass: 'howardKratos'
                        //     }
                        // });

                        // var mailOptions = 
                        // {
                        //     from: 'jamesf6888@gmail.com',
                        //     to: email,
                        //     subject: 'Digital Office | Account Created Successfully!',
                        //     // text: 'This is your pair of keys. Please save them where no one can find them.\n' + privateKey + '\n' + publicKey
                        //     text: privateKey + '\n' + publicKey
                        // };
                          
                        // transporter.sendMail(mailOptions, function(error, info)
                        // {
                        //     if (error) 
                        //     {
                        //         console.log(error);
                        //     } else 
                        //     {
                        //         console.log('Email sent: ' + info.response);
                        //     }
                        // });

                        //Hash password
                        bcrypt.genSalt(10, (err, salt) => 
                            bcrypt.hash(newUser.password, salt, (err, hash) => 
                            {
                                if(err) throw err;
                                // Set password to hashed
                                newUser.password = hash;
                                // Save User
                                newUser.save()
                                    .then(user => 
                                        {
                                            req.flash('success_msg', 'You are now registered and can log in');
                                            res.redirect('/users/login');
                                        })
                                    .catch(err => console.log(err));
                            }));
                    }
                });
    }
});

module.exports.router = router;

// const uuid = require('uuid');
// const members = require('../../Members');


// // Gets all Members
// router.get('/', (req, res) => res.json(members));

// // Get single Member
// router.get('/:id', (req, res) =>
// {
//     const isFound = members.some(member => member.id === parseInt(req.params.id));

//     if(isFound)
//         res.json(members.filter(member => member.id === parseInt(req.params.id)));
//     else
//         res.status(400).json({ msg: `No member with the ID:${req.params.id}`});    
// });

// // Create Member
// router.post('/', (req, res) =>
// {
//     const newMember = 
//     {
//         id: uuid.v4(),
//         firstname: req.body.firstname,
//         lastname: req.body.lastname,
//         username: req.body.username,
//         password: req.body.password,
//         publickey: req.body.publickey,
//         email: req.body.email
//     };

//     if(!newMember.firstname || !newMember.lastname)
//         return res.status(400).json({ msg: 'Please fill all the fields' });
    
//     members.push(newMember);
//     // res.json(members);
//     res.redirect('/');
// });

// // Update member
// router.put('/:id', (req, res) =>
// {
//     const isFound = members.some(member => member.id === parseInt(req.params.id));

//     if(isFound)
//     {
//         const updMember = req.body;
//         members.forEach(member => 
//             {
//                 if(member.id === parseInt(req.params.id))
//                 {
//                     member.firstname = updMember.firstname ? updMember.firstname: member.firstname;
//                     member.lastname = updMember.lastname ? updMember.lastname: member.lastname;
//                     res.json({ msg: 'Member updated', member});
//                 }
//             });
//         res.json(members.filter(member => member.id === parseInt(req.params.id)));
//     }
//     else
//         res.status(400).json({ msg: `No member with the ID:${req.params.id}`});
// });

// // Delete single Member
// router.delete('/:id', (req, res) =>
// {
//     const isFound = members.some(member => member.id === parseInt(req.params.id));

//     if(isFound)
//     {
//         res.json(
//             { 
//                 msg: 'Member deleted',
//                 members: members.filter(member => member.id !== parseInt(req.params.id))
//             });
//     }
//     else
//         res.status(400).json({ msg: `No member with the ID:${req.params.id}`});    
// });